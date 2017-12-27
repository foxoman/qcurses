/*******************************************************************************
 * Copyright 2017 Trent Reed
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *------------------------------------------------------------------------------
 * Slate Terminal UI Source
 ******************************************************************************/

#include "slate.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

////////////////////////////////////////////////////////////////////////////////
// Slate Defines
////////////////////////////////////////////////////////////////////////////////

#define SLATE_DEFAULT_MSGBUFFER 256
#define SLATE_STRING_INTERNAL_SIZE sizeof(char *)

#define __SLATE_CONCAT(a,b) a##b
#define  _SLATE_CONCAT(a,b) __SLATE_CONCAT(a,b)
#define   SLATE_CONCAT(a,b)  _SLATE_CONCAT(a,b)

#define SLATE_MIN(a,b) (((a) < (b)) ? (a) : (b))

#define SLATE_MAX(a,b) (((a) > (b)) ? (a) : (b))

#define slate_array_length(arr) (sizeof(arr) / sizeof(arr[0]))

////////////////////////////////////////////////////////////////////////////////
// Slate Internal Structures
////////////////////////////////////////////////////////////////////////////////

typedef enum slate_message_buffer_t {
  SLATE_MESSAGE_BUFFER_FIRST = 0,
  SLATE_MESSAGE_BUFFER_SECOND = 1,
  SLATE_MESSAGE_BUFFER_RANGE_BEGIN = SLATE_MESSAGE_BUFFER_FIRST,
  SLATE_MESSAGE_BUFFER_RANGE_END = SLATE_MESSAGE_BUFFER_SECOND,
  SLATE_MESSAGE_BUFFER_RANGE_SIZE = (SLATE_MESSAGE_BUFFER_RANGE_END - SLATE_MESSAGE_BUFFER_RANGE_BEGIN + 1),
  SLATE_MESSAGE_BUFFER_MAX_ENUM = 0x7FFF
} slate_message_buffer_t;

#define slate_vector_t(type) struct {                                           \
  type *                                data;                                   \
  uint32_t                              count;                                  \
  uint32_t                              capacity;                               \
}

typedef slate_vector_t(char)                slate_vector_raw_t;
typedef slate_vector_t(slate_signal_t *)    slate_vector_signal_t;
typedef slate_vector_t(slate_slot_t *)      slate_vector_slot_t;
typedef slate_vector_t(slate_widget_t *)    slate_vector_widget_t;

typedef struct slate_signal_registration_t {
  slate_token_t                         token;
  size_t                                offset;
} slate_signal_registration_t;

typedef union slate_string_data_t {
  char                                  pInternal[SLATE_STRING_INTERNAL_SIZE];
  char *                                pExternal;
} slate_string_data_t;

typedef struct slate_string_t {
  slate_string_data_t                   data;
  size_t                                length;
  size_t                                capacity;
} slate_string_t;

typedef struct slate_buffer_t {
  char *                                data;
  size_t                                length;
  size_t                                capacity;
} slate_buffer_t;

struct slate_slot_t {
  void const *                          pfnProcedure;
  slate_widget_t *                      thisWidget;
  slate_vector_signal_t                 signals;
};

struct slate_signal_t {
  slate_token_t                         token;
  slate_widget_t *                      thisWidget;
  slate_vector_slot_t                   slots;
};

struct slate_widget_t {
  slate_widget_type_t                   type;
  slate_alloc_t const *                 pAllocator;
  slate_widget_t *                      pParent;
  slate_bool_t                          isDirty;
  slate_vector_widget_t                 children;
  slate_vector_signal_t                 signals;
  slate_vector_slot_t                   slots;
  uint32_t                              desiredRows;
  uint32_t                              desiredColumns;
};

typedef union slate_application_signals_t {
  slate_signal_t *                      pointers[3];
  struct {
    slate_signal_t *                    onMessage;
    slate_signal_t *                    onResize;
    slate_signal_t *                    onPaint;
  };
} slate_application_signals_t;

struct slate_application_t {
  slate_widget_t                        widget;
  WINDOW *                              mainWindow;
  slate_application_signals_t           signals;
  uint32_t                              screenRows;
  uint32_t                              screenColumns;
  slate_message_buffer_t                previousBuffer;
  slate_message_buffer_t                currentBuffer;
  slate_buffer_t                        messageBuffer[SLATE_MESSAGE_BUFFER_RANGE_SIZE];
  slate_status_bar_t *                  statusBar;
};

struct slate_status_bar_t {
  slate_widget_t                        widget;
};

struct slate_label_t {
  slate_widget_t                        widget;
  slate_align_t                         alignment;
  slate_string_t                        pText;
};

////////////////////////////////////////////////////////////////////////////////
// Slate Internal Variables
////////////////////////////////////////////////////////////////////////////////

static slate_keycode_t const s_keymap[] = {
  SLATE_KEYCODE_INVALID, SLATE_KEYCODE_A, SLATE_KEYCODE_B, SLATE_KEYCODE_C,                                             // 0x00
  SLATE_KEYCODE_D, SLATE_KEYCODE_E, SLATE_KEYCODE_F, SLATE_KEYCODE_G,                                                   // 0x04
  SLATE_KEYCODE_H, SLATE_KEYCODE_I, SLATE_KEYCODE_J, SLATE_KEYCODE_K,                                                   // 0x08
  SLATE_KEYCODE_L, SLATE_KEYCODE_RETURN, SLATE_KEYCODE_N, SLATE_KEYCODE_O,                                              // 0x0C

  SLATE_KEYCODE_P, SLATE_KEYCODE_Q, SLATE_KEYCODE_R, SLATE_KEYCODE_S,                                                   // 0x10
  SLATE_KEYCODE_T, SLATE_KEYCODE_U, SLATE_KEYCODE_V, SLATE_KEYCODE_W,                                                   // 0x14
  SLATE_KEYCODE_X, SLATE_KEYCODE_Y, SLATE_KEYCODE_Z, SLATE_KEYCODE_ESCAPE,                                              // 0x18
  SLATE_KEYCODE_INVALID, SLATE_KEYCODE_INVALID, SLATE_KEYCODE_INVALID, SLATE_KEYCODE_INVALID,                           // 0x1C

  SLATE_KEYCODE_SPACE, SLATE_KEYCODE_1, SLATE_KEYCODE_SINGLE_QUOTE, SLATE_KEYCODE_3,                                    // 0x20
  SLATE_KEYCODE_4, SLATE_KEYCODE_5, SLATE_KEYCODE_7, SLATE_KEYCODE_SINGLE_QUOTE,                                        // 0x24
  SLATE_KEYCODE_9, SLATE_KEYCODE_0, SLATE_KEYCODE_8, SLATE_KEYCODE_EQUALS,                                              // 0x28
  SLATE_KEYCODE_COMMA, SLATE_KEYCODE_MINUS, SLATE_KEYCODE_PERIOD, SLATE_KEYCODE_SOLIDUS,                                // 0x2C

  SLATE_KEYCODE_0, SLATE_KEYCODE_1, SLATE_KEYCODE_2, SLATE_KEYCODE_3,                                                   // 0x30
  SLATE_KEYCODE_4, SLATE_KEYCODE_5, SLATE_KEYCODE_6, SLATE_KEYCODE_7,                                                   // 0x34
  SLATE_KEYCODE_8, SLATE_KEYCODE_9, SLATE_KEYCODE_SEMICOLON, SLATE_KEYCODE_SEMICOLON,                                   // 0x38
  SLATE_KEYCODE_COMMA, SLATE_KEYCODE_EQUALS, SLATE_KEYCODE_PERIOD, SLATE_KEYCODE_SOLIDUS,                               // 0x3C

  SLATE_KEYCODE_2, SLATE_KEYCODE_A, SLATE_KEYCODE_B, SLATE_KEYCODE_C,                                                   // 0x40
  SLATE_KEYCODE_D, SLATE_KEYCODE_E, SLATE_KEYCODE_F, SLATE_KEYCODE_G,                                                   // 0x44
  SLATE_KEYCODE_H, SLATE_KEYCODE_I, SLATE_KEYCODE_J, SLATE_KEYCODE_K,                                                   // 0x48
  SLATE_KEYCODE_L, SLATE_KEYCODE_M, SLATE_KEYCODE_N, SLATE_KEYCODE_O,                                                   // 0x4C

  SLATE_KEYCODE_P, SLATE_KEYCODE_Q, SLATE_KEYCODE_R, SLATE_KEYCODE_S,                                                   // 0x50
  SLATE_KEYCODE_T, SLATE_KEYCODE_U, SLATE_KEYCODE_V, SLATE_KEYCODE_W,                                                   // 0x54
  SLATE_KEYCODE_X, SLATE_KEYCODE_Y, SLATE_KEYCODE_Z, SLATE_KEYCODE_LEFT_BRACKET,                                        // 0x58
  SLATE_KEYCODE_REVERSE_SOLIDUS, SLATE_KEYCODE_RIGHT_BRACKET, SLATE_KEYCODE_6, SLATE_KEYCODE_MINUS,                     // 0x5C

  SLATE_KEYCODE_BACKTICK, SLATE_KEYCODE_A, SLATE_KEYCODE_B, SLATE_KEYCODE_C,                                            // 0x60
  SLATE_KEYCODE_D, SLATE_KEYCODE_E, SLATE_KEYCODE_F, SLATE_KEYCODE_G,                                                   // 0x64
  SLATE_KEYCODE_H, SLATE_KEYCODE_I, SLATE_KEYCODE_J, SLATE_KEYCODE_K,                                                   // 0x68
  SLATE_KEYCODE_L, SLATE_KEYCODE_M, SLATE_KEYCODE_N, SLATE_KEYCODE_O,                                                   // 0x6C

  SLATE_KEYCODE_P, SLATE_KEYCODE_Q, SLATE_KEYCODE_R, SLATE_KEYCODE_S,                                                   // 0x70
  SLATE_KEYCODE_T, SLATE_KEYCODE_U, SLATE_KEYCODE_V, SLATE_KEYCODE_W,                                                   // 0x74
  SLATE_KEYCODE_X, SLATE_KEYCODE_Y, SLATE_KEYCODE_Z, SLATE_KEYCODE_LEFT_BRACKET,                                        // 0x78
  SLATE_KEYCODE_REVERSE_SOLIDUS, SLATE_KEYCODE_RIGHT_BRACKET, SLATE_KEYCODE_BACKTICK, SLATE_KEYCODE_BACKSPACE,          // 0x7C
};

static slate_modifier_t const s_modmap[] = {

  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,       // 0x00
  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,       // 0x04
  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,       // 0x08
  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_NONE, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,              // 0x0C

  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,       // 0x10
  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT,       // 0x14
  SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_CONTROL_BIT, SLATE_MODIFIER_NONE,              // 0x18
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x1C

  SLATE_MODIFIER_NONE, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,                    // 0x20
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_NONE,                    // 0x24
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x28
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x2C

  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x30
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x34
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_NONE,                              // 0x38
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_NONE, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,                    // 0x3C

  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x40
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x44
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x48
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x4C

  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x50
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,               // 0x54
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_NONE,                    // 0x58
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT,                         // 0x5C

  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x60
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x64
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x68
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x6C

  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x70
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE,                                   // 0x74
  SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_NONE, SLATE_MODIFIER_SHIFT_BIT,                              // 0x78
  SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_SHIFT_BIT, SLATE_MODIFIER_NONE,                    // 0x7C
};

////////////////////////////////////////////////////////////////////////////////
// Slate Internal Signals
////////////////////////////////////////////////////////////////////////////////

#define slate_emit(pSignal, pfnType, ...) do {                                  \
  int err;                                                                      \
  uint32_t idx;                                                                 \
  slate_slot_t * pSlot;                                                         \
  for (idx = 0; idx < (pSignal)->slots.count; ++idx) {                          \
    pSlot = (pSignal)->slots.data[idx];                                         \
    err = ((pfnType)pSlot->pfnProcedure)(__VA_ARGS__);                          \
    if (err) {                                                                  \
      return err;                                                               \
    }                                                                           \
  }                                                                             \
  return 0;                                                                     \
} while (0)

#define slate_raise(pWidget, func, ...) do {                                    \
  slate_signal_t * pSignal;                                                     \
  pSignal = slate_widget_find_signal(pWidget, SLATE_CONCAT(func,_token));       \
  if (!pSignal) {                                                               \
    return ENOTSUP;                                                             \
  }                                                                             \
  return func(pSignal, __VA_ARGS__);                                            \
} while (0)

#define    slate_emit_onmessage_token SLATE_TOKEN_ONMESSAGE
static int slate_emit_onmessage (
  slate_signal_t *                      pSignal,
  slate_message_t const *               pMessage
) {
  slate_emit(pSignal, pfn_slate_onmessage, pSlot->thisWidget, pMessage);
}

#define slate_raise_onmessage(pWidget, pMessage) __slate_raise_onmessage((slate_widget_t *)pWidget, pMessage)
static int __slate_raise_onmessage (
  slate_widget_t *                      pWidget,
  slate_message_t const *               pMessage
) {
  slate_raise(pWidget, slate_emit_onmessage, pMessage);
}

#define    slate_emit_onresize_token SLATE_TOKEN_ONRESIZE
static int slate_emit_onresize (
  slate_signal_t *                      pSignal,
  uint32_t                              screenRows,
  uint32_t                              screenColumns
) {
  slate_emit(pSignal, pfn_slate_onresize, pSlot->thisWidget, screenRows, screenColumns);
}

#define slate_raise_onresize(pWidget, screenRows, screenColumns) __slate_raise_onresize((slate_widget_t *)pWidget, screenRows, screenColumns)
static int __slate_raise_onresize (
  slate_widget_t *                      pWidget,
  uint32_t                              screenRows,
  uint32_t                              screenColumns
) {
  slate_raise(pWidget, slate_emit_onresize, screenRows, screenColumns);
}

#define    slate_emit_onpaint_token SLATE_TOKEN_ONPAINT
static int slate_emit_onpaint (
  slate_signal_t *                      pSignal,
  uint32_t                              availRows,
  uint32_t                              availColumns
) {
  slate_emit(pSignal, pfn_slate_onpaint, pSlot->thisWidget, availRows, availColumns);
}

#define slate_raise_onpaint(pWidget, availRows, availColumns) __slate_raise_onpaint((slate_widget_t *)pWidget, availRows, availColumns)
static int __slate_raise_onpaint (
  slate_widget_t *                      pWidget,
  uint32_t                              availRows,
  uint32_t                              availColumns
) {
  slate_raise(pWidget, slate_emit_onpaint, availRows, availColumns);
}

#define    slate_emit_onkey_token SLATE_TOKEN_ONKEY
static int slate_emit_onkey (
  slate_signal_t *                      pSignal,
  slate_keycode_t                       keycode,
  int                                   value
) {
  slate_emit(pSignal, pfn_slate_onkey, pSlot->thisWidget, keycode, value);
}

#define slate_raise_onkey(pWidget, keycode, value) __slate_raise_onkey((slate_widget_t *)pWidget, keycode, value)
static int __slate_raise_onkey (
  slate_widget_t *                      pWidget,
  slate_keycode_t                       keycode,
  int                                   value
) {
  slate_raise(pWidget, slate_emit_onkey, keycode, value);
}

#define    slate_emit_onrecalculate_token SLATE_TOKEN_ONRECALCULATE
static int slate_emit_onrecalculate (
  slate_signal_t *                      pSignal,
  void const *                          pNull
) {
  slate_emit(pSignal, pfn_slate_onrecalculate, pSlot->thisWidget);
}

#define slate_raise_onrecalculate(pWidget) __slate_raise_onrecalculate((slate_widget_t *)pWidget)
static int __slate_raise_onrecalculate (
  slate_widget_t *                      pWidget
) {
  slate_raise(pWidget, slate_emit_onrecalculate, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// Slate Internal Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// Generic Internal Functions
//------------------------------------------------------------------------------

static void * slate_allocate (slate_alloc_t const * pAllocator, size_t n, size_t align) {
  return aligned_alloc(align, n);
}

static void * slate_reallocate (slate_alloc_t const * pAllocator, void * ptr, size_t n) {
  return realloc(ptr, n);
}

static void slate_free (slate_alloc_t const * pAllocator, void * ptr) {
  free(ptr);
}

//------------------------------------------------------------------------------
// Vector Internal Functions
//------------------------------------------------------------------------------

#define slate_vector_full(pVector) ((pVector)->count == (pVector)->capacity)

static int __slate_vector_resize (slate_alloc_t const * pAllocator, slate_vector_raw_t * pVector, size_t elementSize, uint32_t capacity) {
  void * ptr;
  ptr = slate_reallocate(pAllocator, pVector->data, elementSize * capacity);
  if (!ptr) {
    return ENOMEM;
  }
  pVector->data = ptr;
  pVector->capacity = capacity;
  return 0;
}

static int __slate_vector_init (slate_alloc_t const * pAllocator, slate_vector_raw_t * pVector, size_t elementSize, uint32_t initialCapacity) {
  pVector->data = NULL;
  pVector->count = 0;
  pVector->capacity = 0;
  if (initialCapacity != 0) {
    return __slate_vector_resize(pAllocator, pVector, elementSize, initialCapacity);
  }
  return 0;
}

static int __slate_vector_grow (slate_alloc_t const * pAllocator, slate_vector_raw_t * pVector, uint32_t elementSize) {
  uint32_t newCapacity;

  // Double our capacity, handle the empty vector case.
  newCapacity = 2 * pVector->capacity;
  if (newCapacity == 0) {
    newCapacity = 1;
  }
  if (newCapacity < pVector->capacity) {
    return ERANGE;
  }

  return __slate_vector_resize(pAllocator, pVector, elementSize, newCapacity);
}

static int __slate_vector_push (slate_alloc_t const * pAllocator, slate_vector_raw_t * pVector, uint32_t elementSize, void const * pData) {
  int err;
  if (slate_vector_full(pVector)) {
    err = __slate_vector_grow(pAllocator, pVector, elementSize);
    if (err) {
      return err;
    }
  }
  memcpy((pVector)->data + elementSize * pVector->count, pData, elementSize);
  return 0;
}

#define slate_vector_resize(pAllocator, pVector, newCapacity)\
__slate_vector_resize(pAllocator, (slate_vector_raw_t *)pVector, sizeof((pVector)->data[0]), newCapacity)

#define slate_vector_init(pAllocator, pVector, initialCapacity)\
__slate_vector_init(pAllocator, (slate_vector_raw_t *)pVector, sizeof((pVector)->data[0]), initialCapacity)

#define slate_vector_deinit(pAllocator, pVector)\
slate_free(pAllocator, (pVector)->data)

#define slate_vector_grow(pAllocator, pVector)\
__slate_vector_grow(pAllocator, (slate_vector_raw_t *)pVector, sizeof((pVector)->data[0]))

#define slate_vector_push(pAllocator, pVector, element)\
__slate_vector_push(pAllocator, (slate_vector_raw_t *)pVector, sizeof((pVector)->data[0]), &element)

//------------------------------------------------------------------------------
// String Internal Functions
//------------------------------------------------------------------------------

static int slate_string_init (
  slate_string_t *                      pString,
  slate_alloc_t const *                 pAllocator,
  size_t                                initialCapacity
) {
  char * pBuffer;

  pString->length = 0;
  pString->capacity = SLATE_STRING_INTERNAL_SIZE;
  pString->data.pExternal = NULL;

  if (initialCapacity > SLATE_STRING_INTERNAL_SIZE) {
    pBuffer = slate_allocate(pAllocator, initialCapacity, 1);
    if (!pBuffer) {
      return ENOMEM;
    }
    pString->data.pExternal = pBuffer;
    pString->capacity = initialCapacity;
  }

  return 0;
}

#define slate_string_data(pString) \
(((pString)->capacity <= SLATE_STRING_INTERNAL_SIZE) ? (pString)->data.pInternal : (pString)->data.pExternal)

static int slate_string_resize (
  slate_string_t *                      pString,
  slate_alloc_t const *                 pAllocator,
  size_t                                newCapacity
) {
  char * pBuffer;
  slate_string_data_t stringData;

  // If the capacity is already enough, simply return.
  if (newCapacity < pString->capacity) {
    return 0;
  }

  // Allocate the buffer properly for copying into.
  {
    stringData = pString->data;
    if (pString->capacity > SLATE_STRING_INTERNAL_SIZE) {
      pBuffer = pString->data.pExternal;
    }
    else {
      pBuffer = NULL;
    }

    pBuffer = slate_reallocate(pAllocator, pBuffer, newCapacity);
    if (!pBuffer) {
      return ENOMEM;
    }

    pString->data.pExternal = pBuffer;
  }

  // Copy the string data (if needed).
  if (pString->capacity <= SLATE_STRING_INTERNAL_SIZE) {
    memcpy(pBuffer, stringData.pInternal, SLATE_STRING_INTERNAL_SIZE);
  }

  pString->capacity = newCapacity;
  return 0;
}

static int slate_string_set_n (
  slate_string_t *                      pString,
  slate_alloc_t const *                 pAllocator,
  char const *                          text,
  size_t                                n
) {
  int err;

  // Resize the string (this is a no-op if unneeded).
  err = slate_string_resize(pString, pAllocator, n);
  if (err) {
    return err;
  }

  // Copy the text into the new string buffer.
  memcpy(slate_string_data(pString), text, n);
  slate_string_data(pString)[n] = 0;
  pString->length = n;

  return 0;
}

//------------------------------------------------------------------------------
// Application Internal Functions
//------------------------------------------------------------------------------

#define slate_buffer_full(pBuffer) ((pBuffer)->length == (pBuffer)->capacity)
#define slate_buffer_avail(pBuffer, amount) ((pBuffer)->length + amount <= (pBuffer)->capacity)

static int slate_buffer_resize (slate_alloc_t const * pAllocator, slate_buffer_t * pBuffer, size_t capacity) {
  void * ptr;
  ptr = slate_reallocate(pAllocator, pBuffer->data, capacity);
  if (!ptr) {
    return ENOMEM;
  }
  pBuffer->data = ptr;
  pBuffer->capacity = capacity;
  return 0;
}

static int slate_buffer_init (slate_alloc_t const * pAllocator, slate_buffer_t * pBuffer, size_t initialCapacity) {
  pBuffer->data = NULL;
  pBuffer->length = 0;
  pBuffer->capacity = 0;
  if (initialCapacity) {
    return slate_buffer_resize(pAllocator, pBuffer, initialCapacity);
  }
  return 0;
}

static int slate_buffer_grow (slate_alloc_t const * pAllocator, slate_buffer_t * pBuffer) {
  size_t newCapacity;

  // Double our capacity, handle the empty vector case.
  newCapacity = 2 * pBuffer->capacity;
  if (newCapacity == 0) {
    newCapacity = 1;
  }
  if (newCapacity < pBuffer->capacity) {
    return ERANGE;
  }

  return slate_buffer_resize(pAllocator, pBuffer, newCapacity);
}

static slate_message_t * slate_application_reserve_message (
  slate_application_t *                 pApplication,
  size_t                                messageSize
) {
  int err;
  slate_message_t * newMessageBuffer;
  if (!slate_buffer_avail(&pApplication->messageBuffer[pApplication->currentBuffer], messageSize)) {
    err = slate_buffer_grow(pApplication->widget.pAllocator, &pApplication->messageBuffer[pApplication->currentBuffer]);
    if (err) {
      return NULL;
    }
  }
  newMessageBuffer = (slate_message_t *)&pApplication->messageBuffer[pApplication->currentBuffer]
    .data[pApplication->messageBuffer[pApplication->currentBuffer].length];
  newMessageBuffer->asBaseMessage.messageSize = messageSize;
  pApplication->messageBuffer[pApplication->currentBuffer].length += messageSize;
  return newMessageBuffer;
}

static int slate_application_push_input_message (
  slate_application_t *                 pApplication,
  slate_message_key_t const *           pMessage
) {
  void * pReservedMessage;
  pReservedMessage = slate_application_reserve_message(
    pApplication,
    sizeof(slate_message_key_t)
  );
  if (!pReservedMessage) {
    return ENOMEM;
  }
  memcpy(pReservedMessage, pMessage, sizeof(slate_message_key_t));
  return 0;
}

static int slate_application_update_input (
  slate_application_t *                 pApplication
) {
  int err;
  slate_message_key_t key;
  key.base.type = SLATE_MESSAGE_TYPE_KEYBOARD;
  key.base.messageSize = sizeof(slate_message_key_t);

  key.value = getch();
  switch (key.value) {
    case ERR:
      // No input waiting.
      return ENODATA;
    case 'a' ... 'z':
    case '0' ... '9':
      key.code = (slate_keycode_t)key.value;
      break;
    case KEY_LEFT:
      key.code = SLATE_KEYCODE_LEFT;
      break;
    case KEY_RIGHT:
      key.code = SLATE_KEYCODE_RIGHT;
      break;
    case KEY_UP:
      key.code = SLATE_KEYCODE_UP;
      break;
    case KEY_DOWN:
      key.code = SLATE_KEYCODE_DOWN;
      break;
    default:
      key.code = SLATE_KEYCODE_INVALID;
      mvprintw(0, 0, "Unhandled value: 0x%x\n", key.value);
      break;
  }

  err = slate_raise_onkey(pApplication, key.code, key.value);
  if (err) {
    return err;
  }
  err = slate_application_push_input_message(pApplication, &key);
  if (err) {
    return err;
  }

  return 0;
}

#define slate_widget_dirty_recursive(pWidget) __slate_widget_dirty_recursive((slate_widget_t *)pWidget)
static void __slate_widget_dirty_recursive(slate_widget_t * pWidget) {
  uint32_t idx;
  pWidget->isDirty = SLATE_TRUE;
  for (idx = 0; idx < pWidget->children.count; ++idx) {
    __slate_widget_dirty_recursive(pWidget->children.data[idx]);
  }
}

static int slate_application_update_resize (
  slate_application_t *                 pApplication
) {
  int err;
  int x, y;
  slate_message_resize_t * pMessage;

  // Grab the current max x/y.
  err = getmaxyx(stdscr, y, x);
  if (err == ERR) {
    return EFAULT;
  }

  // If they haven't changed from our previous x/y, nothing to do.
  if (pApplication->screenColumns == x && pApplication->screenRows == y) {
    return 0;
  }

  // We always update what the application knows as its screen size.
  pApplication->screenRows = (uint32_t)y;
  pApplication->screenColumns = (uint32_t)x;

  // If we are supposed to post a message, do so.
  err = slate_raise_onresize(pApplication, pApplication->screenRows, pApplication->screenColumns);
  if (err) {
    return err;
  }
  pMessage = (slate_message_resize_t *)slate_application_reserve_message(
    pApplication,
    sizeof(slate_message_resize_t)
  );
  if (!pMessage) {
    return ENOMEM;
  }
  pMessage->base.type = SLATE_MESSAGE_TYPE_RESIZE;
  pMessage->characterRows = pApplication->screenRows;
  pMessage->characterColumns = pApplication->screenColumns;

  // Mark all widgets as dirty.
  if (pApplication->statusBar) {
    slate_widget_dirty_recursive(pApplication->statusBar);
  }

  clear();

  return 0;
}

static int slate_application_update_message (
  slate_application_t *                 pApplication,
  slate_message_t const *               pMessage
) {
  return slate_raise_onmessage(pApplication, pMessage);
}

static int slate_application_update_visuals (
  slate_application_t *                 pApplication
) {
  int err;
  uint32_t idx;
  uint32_t availRows;
  uint32_t eachRows;

  // Redraw the central widget.
  if (pApplication->widget.children.count) {
    move(0, 0);
    availRows = pApplication->screenRows;
    if (pApplication->statusBar) {
      --availRows;
    }
    eachRows = availRows / pApplication->widget.children.count;
    for (idx = 0; idx < pApplication->widget.children.count; ++idx) {
      if (idx + 1 == pApplication->widget.children.count) {
        eachRows = availRows;
      }
      else {
        availRows -= eachRows;
      }
      err = slate_raise_onpaint(pApplication->widget.children.data[idx], eachRows, pApplication->screenColumns);
      if (err) {
        return err;
      }
    }
  }

  // If there is a status bar, move to the bottom of the screen and paint it.
  if (pApplication->statusBar) {
    move(pApplication->screenRows - 1, 0);
    err = slate_raise_onpaint(pApplication->statusBar, 1, pApplication->screenColumns);
    if (err) {
      return err;
    }
  }

  return 0;
}

static int slate_application_update (
  slate_application_t *                 pApplication
) {
  int err;
  size_t byte;

  // Process all pending input (if available).
  do {
    err = slate_application_update_input(pApplication);
  } while (!err);
  if (err != ENODATA) {
    return err;
  }

  // Process pending resize (if resized).
  err = slate_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  // Swap the message buffers so that we can process the next message set.
  pApplication->previousBuffer = pApplication->currentBuffer;
  ++pApplication->currentBuffer;
  if (pApplication->currentBuffer > SLATE_MESSAGE_BUFFER_RANGE_END) {
    pApplication->currentBuffer = SLATE_MESSAGE_BUFFER_RANGE_BEGIN;
  }

  // Send all of the pending messages out to those listening.
  slate_message_t const * message;
  slate_buffer_t * pMessageBuffer = &pApplication->messageBuffer[pApplication->previousBuffer];
  for (byte = 0; byte < pMessageBuffer->length; byte += message->asBaseMessage.messageSize) {
    message = (slate_message_t const*)&pMessageBuffer->data[byte];
    err = slate_application_update_message(pApplication, message);
    if (err) {
      return err;
    }
  }
  pMessageBuffer->length = 0;

  // Handle the visual update iff the application is marked dirty.
  if (pApplication->widget.isDirty) {
    err = slate_application_update_visuals(pApplication);
    if (err) {
      return err;
    }
    refresh();
  }

  // TODO: Have the application sleep a little bit to save power.

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Slate API Functions
////////////////////////////////////////////////////////////////////////////////

static int __slate_signal_connect (
  slate_alloc_t const *                 pAllocator,
  slate_signal_t *                      pSignal,
  slate_slot_t *                        pSlot
) {
  int err;

  // If the array of slots is full, grow it.
  if (slate_vector_full(&pSignal->slots)) {
    err = slate_vector_grow(pAllocator, &pSignal->slots);
    if (err) {
      return err;
    }
  }

  // If the array of signals is full, grow it.
  if (slate_vector_full(&pSlot->signals)) {
    err = slate_vector_grow(pAllocator, &pSlot->signals);
    if (err) {
      return err;
    }
  }

  // Hook up the signal to the slot and vice-versa.
  pSignal->slots.data[pSignal->slots.count++] = pSlot;
  pSlot->signals.data[pSlot->signals.count++] = pSignal;

  return 0;
}

//------------------------------------------------------------------------------
// Widget API Functions
//------------------------------------------------------------------------------

void __slate_widget_init (
  slate_widget_t *                      pWidget,
  slate_alloc_t const *                 pAllocator,
  slate_widget_type_t                   widgetType,
  size_t                                widgetSize
) {
  memset(pWidget, 0, widgetSize);
  pWidget->pAllocator = pAllocator;
  pWidget->type = widgetType;
  pWidget->pParent = NULL;
  slate_vector_init(pAllocator, &pWidget->children, 0);
  slate_vector_init(pAllocator, &pWidget->signals, 0);
  slate_vector_init(pAllocator, &pWidget->slots, 0);
}

slate_signal_t * __slate_widget_find_signal (
  slate_widget_t *                      pSource,
  slate_token_t                         token
) {
  uint32_t idx;
  for (idx = 0; idx < pSource->signals.count; ++idx) {
    if (pSource->signals.data[idx]->token == token) {
      return pSource->signals.data[idx];
    }
  }
  return NULL;
}

slate_slot_t * __slate_widget_find_slot (
  slate_widget_t *                      pSource,
  void const *                          pfnProcedure
) {
  uint32_t idx;
  for (idx = 0; idx < pSource->slots.count; ++idx) {
    if (pSource->slots.data[idx]->pfnProcedure == pfnProcedure) {
      return pSource->slots.data[idx];
    }
  }
  return NULL;
}

int __slate_widget_add_signal (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  slate_signal_t **                     pSignal
) {
  int err;
  slate_signal_t * signal;

  // Grow the signal array if needed.
  if (slate_vector_full(&pWidget->signals)) {
    err = slate_vector_grow(pWidget->pAllocator, &pWidget->signals);
    if (err) {
      return err;
    }
  }

  // Add a new widget to the signal array.
  signal = slate_allocate(
    pWidget->pAllocator,
    sizeof(slate_signal_t),
    1
  );
  if (!signal) {
    return ENOMEM;
  }

  // Initialize the signal, and add it to the widget.
  signal->token = token;
  signal->thisWidget = pWidget;
  slate_vector_init(pWidget->pAllocator, &signal->slots, 0);
  pWidget->signals.data[pWidget->signals.count++] = signal;

  if (pSignal) {
    *pSignal = signal;
  }
  return 0;
}

int __slate_widget_add_slot (
  slate_widget_t *                      pWidget,
  void const *                          pfnProcedure,
  slate_slot_t **                       pSlot
) {
  int err;
  slate_slot_t * slot;

  // Grow the slot array if needed.
  if (slate_vector_full(&pWidget->slots)) {
    err = slate_vector_grow(pWidget->pAllocator, &pWidget->slots);
    if (err) {
      return err;
    }
  }

  // Add a new widget to the slot array.
  slot = slate_allocate(
    pWidget->pAllocator,
    sizeof(slate_slot_t),
    1
  );
  if (!slot) {
    return ENOMEM;
  }

  // Initialize the slot, and add it to the widget.
  slot->thisWidget = pWidget;
  slot->pfnProcedure = pfnProcedure;
  slate_vector_init(pWidget->pAllocator, &slot->signals, 0);
  pWidget->slots.data[pWidget->slots.count++] = slot;

  if (pSlot) {
    *pSlot = slot;
  }
  return 0;
}

int __slate_widget_add_connect (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  void const *                          pfnProcedure
) {
  int err;
  slate_signal_t * signal;
  slate_slot_t * slot;

  // Add the signal to the widget.
  err = __slate_widget_add_signal(pWidget, token, &signal);
  if (err) {
    return err;
  }

  // Add the slot to the widget.
  err = __slate_widget_add_slot(pWidget, pfnProcedure, &slot);
  if (err) {
    return err;
  }

  // Connect the signal to the slot.
  return __slate_signal_connect(pWidget->pAllocator, signal, slot);
}

int __slate_widget_find_or_add_signal (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  slate_signal_t **                     pSignal
) {
  slate_signal_t * signal;

  signal = __slate_widget_find_signal(pWidget, token);
  if (!signal) {
    return __slate_widget_add_signal(pWidget, token, pSignal);
  }

  *pSignal = signal;
  return 0;
}

int __slate_widget_find_or_add_slot (
  slate_widget_t *                      pWidget,
  void const *                          pfnProcedure,
  slate_slot_t **                       pSlot
) {
  slate_slot_t * slot;

  slot = __slate_widget_find_slot(pWidget, pfnProcedure);
  if (!slot) {
    return __slate_widget_add_slot(pWidget, pfnProcedure, pSlot);
  }

  *pSlot = slot;
  return 0;
}

int __slate_widget_connect (
  slate_widget_t *                      pSource,
  slate_token_t                         signalName,
  slate_widget_t *                      pTarget,
  void const *                          pSlotFunc
) {
  int err;
  slate_signal_t * pSourceSignal;
  slate_slot_t * pTargetSlot;

  // See if we can find the signal so that we can connect to it.
  pSourceSignal = __slate_widget_find_signal(pSource, signalName);
  if (!pSourceSignal) {
    return ENOTSUP;
  }

  // See if we can find the slot so that we can connect to it.
  // If we cannot find it, we should add it and ensure it was created.
  err = __slate_widget_find_or_add_slot(pTarget, pSlotFunc, &pTargetSlot);
  if (err) {
    return err;
  }

  // Connect the signal to the slot.
  return __slate_signal_connect(pSource->pAllocator, pSourceSignal, pTargetSlot);
}

int __slate_widget_insert (
  slate_widget_t *                      pParent,
  slate_widget_t *                      pChild
) {
  int err;
  if (pChild->pParent != NULL) {
    return EINVAL;
  }
  err = slate_vector_push(pParent->pAllocator, &pParent->children, pChild);
  if (err) {
    return err;
  }
  pChild->pParent = pParent;
  ++pParent->children.count;
  return 0;
}

void __slate_widget_dirty (
  slate_widget_t *                      pWidget
) {
  while (pWidget) {
    pWidget->isDirty = SLATE_TRUE;
    pWidget = pWidget->pParent;
  }
}

//------------------------------------------------------------------------------
// Application API Functions
//------------------------------------------------------------------------------

int slate_create_application (
  slate_application_info_t const *      pCreateInfo,
  slate_application_t **                pApplication
) {
  int err;
  uint32_t idx;
  slate_application_t * application;

  // Allocate the terminal UI application.
  application = slate_allocate(
    pCreateInfo->pAllocator,
    sizeof(slate_application_t),
    1
  );
  if (!application) {
    return ENOMEM;
  }

  // Raw initialization of the widget (in case we need to destroy prematurely).
  slate_widget_init(application, pCreateInfo->pAllocator, SLATE_WIDGET_TYPE_APPLICATION);

  // Allocate the default signal list.
  err = slate_vector_init(
    pCreateInfo->pAllocator,
    &application->widget.signals,
    1
  );
  if (err) {
    return err;
  }

  // Allocate the dynamic message buffer.
  for (idx = SLATE_MESSAGE_BUFFER_RANGE_BEGIN; idx <= SLATE_MESSAGE_BUFFER_RANGE_END; ++idx) {
    err = slate_buffer_init(
      pCreateInfo->pAllocator,
      &application->messageBuffer[idx],
      SLATE_DEFAULT_MSGBUFFER
    );
    if (err) {
      return err;
    }
  }

  // Initialize the application so that it can run.
  application->currentBuffer = SLATE_MESSAGE_BUFFER_RANGE_BEGIN;
  application->previousBuffer = SLATE_MESSAGE_BUFFER_RANGE_END;

  // Create all of the signals/slots
  slate_widget_add_signal(application, SLATE_APPLICATION_ONKEY, NULL);
  slate_widget_add_signal(application, SLATE_APPLICATION_ONMESSAGE, NULL);
  slate_widget_add_signal(application, SLATE_APPLICATION_ONRESIZE, NULL);
  slate_widget_add_signal(application, SLATE_APPLICATION_ONPAINT, NULL);

  // Return the application to the caller.
  *pApplication = application;
  return 0;
}

int slate_destroy_application (
  slate_application_t *                 pApplication
) {
  slate_free(pApplication->widget.pAllocator, pApplication);
  return 0;
}

static int slate_application_start (
  slate_application_t *                 pApplication
) {
  int err;

  // Construct the main window
  pApplication->mainWindow = initscr();
  if (!pApplication->mainWindow) {
    return EFAULT;
  }

  // Attempt to enable the special keys (arrows, function, etc.)
  err = keypad(stdscr, TRUE);
  if (err == ERR) {
    return EFAULT;
  }

  // Don't echo the keys as they're pressed to the screen.
  err = noecho();
  if (err == ERR) {
    return EFAULT;
  }

  // Don't wait for data if there are no keys pressed.
  err = nodelay(pApplication->mainWindow, TRUE);
  if (err == ERR) {
    return EFAULT;
  }

  return 0;
}

static int slate_application_end (
  slate_application_t *                 pApplication
) {
  int err;
  err = endwin();
  if (err == ERR) {
    return EFAULT;
  }
  return 0;
}

int slate_run_application (
  slate_application_t *                 pApplication
) {
  int err;

  err = slate_application_start(pApplication);
  if (err) {
    return err;
  }

  // Grab the terminal sizes without issuing a resize event.
  err = slate_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  do {
    err = slate_application_update(pApplication);
  } while (!err);

  (void)slate_application_end(pApplication);

  return err;
}

int slate_application_set_status_bar (
  slate_application_t *                 pApplication,
  slate_status_bar_t *                  pStatusBar
) {
  pStatusBar->widget.pParent = &pApplication->widget;
  pApplication->statusBar = pStatusBar;
  return 0;
}

//------------------------------------------------------------------------------
// Status Bar API Functions
//------------------------------------------------------------------------------

static int slate_status_bar_onrecalculate (
  slate_status_bar_t *                  pStatusBar
) {
  int err;
  uint32_t idx;
  slate_widget_t * pWidget;

  pStatusBar->widget.desiredRows = 1;
  pStatusBar->widget.desiredColumns = 0;

  // Status bar is horizontal-spaced, print each element in order.
  for (idx = 0; idx < pStatusBar->widget.children.count; ++idx) {
    pWidget = pStatusBar->widget.children.data[idx];
    err = slate_raise_onrecalculate(pWidget);
    if (err) {
      return err;
    }
    pStatusBar->widget.desiredColumns += pWidget->desiredColumns;
  }

  return 0;
}

static int slate_status_bar_onpaint (
  slate_status_bar_t *                  pStatusBar,
  uint32_t                              availRows,
  uint32_t                              availColumns
) {
  int err;
  uint32_t idx;
  slate_widget_t * pWidget;
  uint32_t amountColumns;


  // Status bar is horizontal-spaced, print each element in order.
  amountColumns = availColumns / pStatusBar->widget.children.count;
  for (idx = 0; idx < pStatusBar->widget.children.count; ++idx) {
    pWidget = pStatusBar->widget.children.data[idx];
    if (idx + 1 == pStatusBar->widget.children.count) {
      amountColumns = availColumns;
    }
    else {
      availColumns -= amountColumns;
    }
    err = slate_raise_onpaint(pWidget, availRows, amountColumns);
    if (err) {
      return err;
    }
  }

  return 0;
}

int slate_create_status_bar (
  slate_alloc_t const *                 pAllocator,
  slate_status_bar_t **                 pStatusBar
) {
  slate_status_bar_t * statusBar;

  // Allocate information for the status bar.
  statusBar = slate_allocate(
    pAllocator,
    sizeof(slate_status_bar_t),
    1
  );
  if (!statusBar) {
    return ENOMEM;
  }

  // Initialize the status bar information.
  slate_widget_init(statusBar, pAllocator, SLATE_WIDGET_TYPE_STATUS_BAR);

  // Create all of the signals/slots
  slate_widget_add_connect(statusBar, SLATE_LABEL_ONRECALCULATE, &slate_status_bar_onrecalculate);
  slate_widget_add_connect(statusBar, SLATE_LABEL_ONPAINT, &slate_status_bar_onpaint);

  *pStatusBar = statusBar;
  return 0;
}

//------------------------------------------------------------------------------
// Label API Functions
//------------------------------------------------------------------------------

static int slate_label_onrecalculate (
  slate_label_t *                       pLabel
) {
  pLabel->widget.desiredRows = 1;
  pLabel->widget.desiredColumns = (uint32_t)pLabel->pText.length;
  return 0;
}

static int slate_label_onpaint (
  slate_label_t *                       pLabel,
  uint32_t                              availRows,
  uint32_t                              availColumns
) {
  int err;
  size_t idx;
  size_t textLength;
  size_t paddingLength;
  slate_align_bits_t halign;
  slate_align_bits_t valign;
  (void)availRows;

  // Grab the required formatting parameters.
  textLength = SLATE_MIN(pLabel->pText.length, availColumns);
  halign = (slate_align_bits_t)(pLabel->alignment & SLATE_ALIGN_HORIZONTAL_MASK);
  valign = (slate_align_bits_t)(pLabel->alignment & SLATE_ALIGN_VERTICAL_MASK);

  // TODO: Handle Multi-line labels.
  // Move in the required number of rows for the provided alignment.
  switch (valign) {
    case SLATE_ALIGN_TOP_BIT:
      paddingLength = 0;
      break;
    default:
    case SLATE_ALIGN_MIDDLE_BIT:
      paddingLength = (availRows / 2);
      if (paddingLength) {
        paddingLength -= 1;
      }
      break;
    case SLATE_ALIGN_BOTTOM_BIT:
      paddingLength = availRows - 1;
      break;
  }
  for (idx = 0; idx < paddingLength; ++idx) {
    addch('\n');
  }

  // Move in the required number of spaces for the provided alignment.
  switch (halign) {
    case SLATE_ALIGN_LEFT_BIT:
      paddingLength = 0;
      break;
    default:
    case SLATE_ALIGN_CENTER_BIT:
      paddingLength = (availColumns / 2) - (textLength / 2);
      break;
    case SLATE_ALIGN_RIGHT_BIT:
      paddingLength = availColumns - textLength;
      break;
  }
  for (idx = 0; idx < paddingLength; ++idx) {
    addch(' ');
  }

  // Write the string to the buffer.
  // NOTE: Ignore return value here, addnstr returns ERR if it cannot move into
  //       the space following the added string, which is impossible to do if right-aligned.
  (void)addnstr(slate_string_data(&pLabel->pText), (int)textLength);

  // Move in the required number of spaces for the provided alignment.
  switch (halign) {
    case SLATE_ALIGN_LEFT_BIT:
      paddingLength = availColumns - textLength;
      break;
    default:
    case SLATE_ALIGN_CENTER_BIT:
      paddingLength = (availColumns / 2) - (textLength / 2);
      break;
    case SLATE_ALIGN_RIGHT_BIT:
      paddingLength = 0;
      break;
  }
  for (idx = 0; idx < paddingLength; ++idx) {
    addch(' ');
  }

  // Move in the required number of rows for the provided alignment.
  switch (valign) {
    case SLATE_ALIGN_TOP_BIT:
      paddingLength = availRows - 1;
      break;
    default:
    case SLATE_ALIGN_MIDDLE_BIT:
      paddingLength = (availRows / 2);
      if (paddingLength) {
        paddingLength -= 1;
      }
      break;
    case SLATE_ALIGN_BOTTOM_BIT:
      paddingLength = 0;
      break;
  }
  for (idx = 0; idx < paddingLength; ++idx) {
    addch('\n');
  }

  return 0;
}

int slate_create_label (
  slate_alloc_t const *                 pAllocator,
  slate_label_t **                      pLabel
) {
  slate_label_t * label;

  // Allocate information for the status bar.
  label = slate_allocate(
    pAllocator,
    sizeof(slate_label_t),
    1
  );
  if (!label) {
    return ENOMEM;
  }

  // Initialize the status bar information.
  slate_widget_init(label, pAllocator, SLATE_WIDGET_TYPE_LABEL);
  slate_string_init(&label->pText, pAllocator, 0);

  // Create all of the signals/slots
  slate_widget_add_connect(label, SLATE_LABEL_ONRECALCULATE, &slate_label_onrecalculate);
  slate_widget_add_connect(label, SLATE_LABEL_ONPAINT, &slate_label_onpaint);

  *pLabel = label;
  return 0;
}

int slate_label_set_align (
  slate_label_t *                       pLabel,
  slate_align_t                         alignment
) {
  if (pLabel->alignment != alignment) {
    pLabel->alignment = alignment;
    slate_widget_dirty(pLabel);
  }
  return 0;
}

int slate_label_set_text (
  slate_label_t *                       pLabel,
  char const *                          text
) {
  return slate_label_set_text_n(pLabel, text, strlen(text));
}

int slate_label_set_text_n (
  slate_label_t *                       pLabel,
  char const *                          text,
  size_t                                n
) {
  int err;

  // Attempt to set the string inside the label.
  err = slate_string_set_n(&pLabel->pText, pLabel->widget.pAllocator, text, n);
  if (err) {
    return err;
  }

  // If we succeeded, we should mark the widget as dirty.
  slate_widget_dirty(pLabel);
  return 0;
}
