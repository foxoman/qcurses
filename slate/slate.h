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
 * Slate Terminal UI Include Header
 ******************************************************************************/
#ifndef   SLATE_H
#define   SLATE_H

#include <stdlib.h>
#include <stdint.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Defines
////////////////////////////////////////////////////////////////////////////////

#define SLATE_HANDLE(name) typedef struct name name

#define SLATE_TERMINATE -1

#define SLATECALL
#define SLATEPTR

typedef uint32_t slate_flags_t;
typedef uint64_t slate_token_t;

#define SLATE_INVALID_ENUM_STRING "<invalid>"

#define SLATE_FALSE 0
#define SLATE_TRUE 1

#define SLATE_TOKENIZE(a,b,c,d,e,f,g,h)                                         \
(((slate_token_t)(a) << 56) | ((slate_token_t)(b) << 48) |                      \
 ((slate_token_t)(c) << 40) | ((slate_token_t)(d) << 32) |                      \
 ((slate_token_t)(e) << 24) | ((slate_token_t)(f) << 16) |                      \
 ((slate_token_t)(g) << 8 ) | ((slate_token_t)(h) << 0 ))

typedef uint_fast8_t slate_bool_t;

////////////////////////////////////////////////////////////////////////////////
// Slate Enumerations
////////////////////////////////////////////////////////////////////////////////

typedef enum slate_message_type_t {
  SLATE_MESSAGE_TYPE_KEYBOARD = 0,
  SLATE_MESSAGE_TYPE_REDRAW = 1,
  SLATE_MESSAGE_TYPE_RESIZE = 2,
  SLATE_MESSAGE_TYPE_BEGIN_RANGE = SLATE_MESSAGE_TYPE_KEYBOARD,
  SLATE_MESSAGE_TYPE_END_RANGE = SLATE_MESSAGE_TYPE_RESIZE,
  SLATE_MESSAGE_TYPE_RANGE_SIZE = (SLATE_MESSAGE_TYPE_END_RANGE - SLATE_MESSAGE_TYPE_BEGIN_RANGE + 1),
  SLATE_MESSAGE_TYPE_MAX_ENUM = 0x7FFF
} slate_message_type_t;

typedef enum slate_keycode_t {
  SLATE_KEYCODE_INVALID = 0x00,
  SLATE_KEYCODE_TAB = 0x09,
  SLATE_KEYCODE_RETURN = 0x0D,
  SLATE_KEYCODE_ESCAPE = 0x1B,
  SLATE_KEYCODE_SPACE = 0x20,
  SLATE_KEYCODE_SINGLE_QUOTE = 0x27,
  SLATE_KEYCODE_PLUS = 0x2B,
  SLATE_KEYCODE_COMMA = 0x2C,
  SLATE_KEYCODE_MINUS = 0x2D,
  SLATE_KEYCODE_PERIOD = 0x2E,
  SLATE_KEYCODE_SOLIDUS = 0x2F,
  SLATE_KEYCODE_0 = '0',
  SLATE_KEYCODE_1 = '1',
  SLATE_KEYCODE_2 = '2',
  SLATE_KEYCODE_3 = '3',
  SLATE_KEYCODE_4 = '4',
  SLATE_KEYCODE_5 = '5',
  SLATE_KEYCODE_6 = '6',
  SLATE_KEYCODE_7 = '7',
  SLATE_KEYCODE_8 = '8',
  SLATE_KEYCODE_9 = '9',
  SLATE_KEYCODE_SEMICOLON = 0x3B,
  SLATE_KEYCODE_EQUALS = 0x3D,
  SLATE_KEYCODE_A = 'a',
  SLATE_KEYCODE_B = 'b',
  SLATE_KEYCODE_C = 'c',
  SLATE_KEYCODE_D = 'd',
  SLATE_KEYCODE_E = 'e',
  SLATE_KEYCODE_F = 'f',
  SLATE_KEYCODE_G = 'g',
  SLATE_KEYCODE_H = 'h',
  SLATE_KEYCODE_I = 'i',
  SLATE_KEYCODE_J = 'j',
  SLATE_KEYCODE_K = 'k',
  SLATE_KEYCODE_L = 'l',
  SLATE_KEYCODE_M = 'm',
  SLATE_KEYCODE_N = 'n',
  SLATE_KEYCODE_O = 'o',
  SLATE_KEYCODE_P = 'p',
  SLATE_KEYCODE_Q = 'q',
  SLATE_KEYCODE_R = 'r',
  SLATE_KEYCODE_S = 's',
  SLATE_KEYCODE_T = 't',
  SLATE_KEYCODE_U = 'u',
  SLATE_KEYCODE_V = 'v',
  SLATE_KEYCODE_W = 'w',
  SLATE_KEYCODE_X = 'x',
  SLATE_KEYCODE_Y = 'y',
  SLATE_KEYCODE_Z = 'z',
  SLATE_KEYCODE_LEFT_BRACKET = 0x5B,
  SLATE_KEYCODE_RIGHT_BRACKET = 0x5D,
  SLATE_KEYCODE_REVERSE_SOLIDUS = 0x5C,
  SLATE_KEYCODE_BACKTICK = 0x60,
  SLATE_KEYCODE_BACKSPACE = 0x7F,

  // Escape Sequences:
  // TODO: Partial support.
  SLATE_KEYCODE_INSERT = 0x100,
  SLATE_KEYCODE_DELETE,
  SLATE_KEYCODE_PAGE_UP,
  SLATE_KEYCODE_PAGE_DOWN,
  SLATE_KEYCODE_LEFT,
  SLATE_KEYCODE_RIGHT,
  SLATE_KEYCODE_UP,
  SLATE_KEYCODE_DOWN,
  SLATE_KEYCODE_END,
  SLATE_KEYCODE_HOME,

  // TODO: Add support.
  SLATE_KEYCODE_NUM_0,
  SLATE_KEYCODE_NUM_1,
  SLATE_KEYCODE_NUM_2,
  SLATE_KEYCODE_NUM_3,
  SLATE_KEYCODE_NUM_4,
  SLATE_KEYCODE_NUM_5,
  SLATE_KEYCODE_NUM_6,
  SLATE_KEYCODE_NUM_7,
  SLATE_KEYCODE_NUM_8,
  SLATE_KEYCODE_NUM_9,
} slate_keycode_t;

typedef enum slate_modifier_bits_t {
  SLATE_MODIFIER_SHIFT_BIT = 0x01,
  SLATE_MODIFIER_CONTROL_BIT = 0x02,
} slate_modifier_bits_t;
typedef slate_flags_t slate_modifier_t;
#define SLATE_MODIFIER_NONE 0

typedef enum slate_widget_type_t {
  SLATE_WIDGET_TYPE_APPLICATION = 0,
  SLATE_WIDGET_TYPE_STATUS_BAR = 1,
  SLATE_WIDGET_TYPE_MENU = 2,
  SLATE_WIDGET_TYPE_LABEL = 3,
} slate_widget_type_t;

typedef enum slate_align_bits_t {
  // Mutually-exclusive horizontal bits:
  SLATE_ALIGN_LEFT_BIT = 0x0001,
  SLATE_ALIGN_CENTER_BIT = 0x0002,
  SLATE_ALIGN_RIGHT_BIT = 0x0003,
  // Mutually-exclusive vertical bits:
  SLATE_ALIGN_TOP_BIT = 0x0010,
  SLATE_ALIGN_MIDDLE_BIT = 0x0020,
  SLATE_ALIGN_BOTTOM_BIT = 0x0030,
} slate_align_bits_t;
typedef slate_flags_t slate_align_t;

#define SLATE_ALIGN_HORIZONTAL_MASK 0x000F
#define SLATE_ALIGN_VERTICAL_MASK   0x00F0

////////////////////////////////////////////////////////////////////////////////
// Slate Messages
////////////////////////////////////////////////////////////////////////////////

typedef struct slate_message_base_t {
  slate_message_type_t                  type;
  size_t                                messageSize;
} slate_message_base_t;

typedef struct slate_message_key_t {
  slate_message_base_t                  base;
  slate_keycode_t                       code;
  slate_modifier_t                      modifier;
  int                                   value;
} slate_message_key_t;

typedef struct slate_message_redraw_t {
  slate_message_base_t                  base;
  uint32_t                              topLeftX;
  uint32_t                              topLeftY;
  uint32_t                              regionWidth;
  uint32_t                              regionHeight;
} slate_message_redraw_t;

typedef struct slate_message_resize_t {
  slate_message_base_t                  base;
  uint32_t                              characterColumns;
  uint32_t                              characterRows;
} slate_message_resize_t;

typedef union slate_message_t {
  slate_message_type_t                  messageType;
  slate_message_base_t                  asBaseMessage;
  slate_message_key_t                   asKeyMessage;
  slate_message_redraw_t                asRedrawMessage;
  slate_message_resize_t                asResizeMessage;
} slate_message_t;

////////////////////////////////////////////////////////////////////////////////
// Slate Structures
////////////////////////////////////////////////////////////////////////////////

SLATE_HANDLE(slate_signal_t);
SLATE_HANDLE(slate_slot_t);
SLATE_HANDLE(slate_widget_t);
SLATE_HANDLE(slate_application_t);
SLATE_HANDLE(slate_status_bar_t);
SLATE_HANDLE(slate_label_t);

typedef void* (SLATEPTR *slate_alloc_pfn)(size_t n, size_t align);
typedef void* (SLATEPTR *slate_realloc_pfn)(void * ptr, size_t n);
typedef void  (SLATEPTR *slate_free_pfn)(void * ptr);

typedef struct slate_alloc_t {
  slate_alloc_pfn                       pfnAllocate;
  slate_realloc_pfn                     pfnReallocate;
  slate_free_pfn                        pfnFree;
  void *                                pUserData;
} slate_alloc_t;

typedef int (*slate_message_proc_t)(slate_message_t const *);

typedef struct slate_application_info_t {
  slate_alloc_t const *                 pAllocator;
  char const *                          pApplicationName;
  char const *                          pDescription;
  char const *                          pVersion;
  char const *                          pCopyright;
} slate_application_info_t;

////////////////////////////////////////////////////////////////////////////////
// Slate Signals
////////////////////////////////////////////////////////////////////////////////

typedef int (*pfn_slate_onkey)(slate_widget_t * pTarget, slate_keycode_t code, int value);
#define SLATE_TOKEN_ONKEY               SLATE_TOKENIZE('_','_','_','_','_','k','e','y')
typedef int (*pfn_slate_onmessage)(slate_widget_t * pTarget, slate_message_t const * pMessage);
#define SLATE_TOKEN_ONMESSAGE           SLATE_TOKENIZE('_','m','e','s','s','a','g','e')
typedef int (*pfn_slate_onresize)(slate_widget_t * pTarget, uint32_t screenRows, uint32_t screenColumns);
#define SLATE_TOKEN_ONRESIZE            SLATE_TOKENIZE('_','_','r','e','s','i','z','e')
typedef int (*pfn_slate_onrecalculate)(slate_widget_t * pTarget);
#define SLATE_TOKEN_ONRECALCULATE       SLATE_TOKENIZE('_','_','r','e','c','a','l','c')
typedef int (*pfn_slate_onpaint)(slate_widget_t * pTarget, uint32_t availRows, uint32_t availColumns);
#define SLATE_TOKEN_ONPAINT             SLATE_TOKENIZE('_','_','_','p','a','i','n','t')

// Application Signals
#define SLATE_APPLICATION_ONKEY         SLATE_TOKEN_ONKEY
#define SLATE_APPLICATION_ONMESSAGE     SLATE_TOKEN_ONMESSAGE
#define SLATE_APPLICATION_ONRESIZE      SLATE_TOKEN_ONRESIZE
#define SLATE_APPLICATION_ONRECALCULATE SLATE_TOKEN_ONRECALCULATE
#define SLATE_APPLICATION_ONPAINT       SLATE_TOKEN_ONPAINT

// Status Bar Signals
#define SLATE_STATUS_BAR_ONRECALCULATE  SLATE_TOKEN_ONRECALCULATE
#define SLATE_STATUS_BAR_ONPAINT        SLATE_TOKEN_ONPAINT

// Label Signals
#define SLATE_LABEL_ONRECALCULATE       SLATE_TOKEN_ONRECALCULATE
#define SLATE_LABEL_ONPAINT             SLATE_TOKEN_ONPAINT

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

char const * slate_message_string (
  slate_message_type_t                  messageType
);

// slate_widget_t'

void __slate_widget_init (
  slate_widget_t *                      pWidget,
  slate_alloc_t const *                 pAllocator,
  slate_widget_type_t                   widgetType,
  size_t                                widgetSize
);

slate_signal_t * __slate_widget_find_signal (
  slate_widget_t *                      pWidget,
  slate_token_t                         token
);

slate_slot_t * __slate_widget_find_slot (
  slate_widget_t *                      pWidget,
  void const *                          pfnProcedure
);

int __slate_widget_add_signal (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  slate_signal_t **                     pSignal
);

int __slate_widget_add_slot (
  slate_widget_t *                      pWidget,
  void const *                          pfnProcedure,
  slate_slot_t **                       pSlot
);

int __slate_widget_add_connect (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  void const *                          pfnProcedure
);

int __slate_widget_find_or_add_signal (
  slate_widget_t *                      pWidget,
  slate_token_t                         token,
  slate_signal_t **                     pSignal
);

int __slate_widget_find_or_add_slot (
  slate_widget_t *                      pWidget,
  void const *                          pfnProcedure,
  slate_slot_t **                       pSlot
);

int __slate_widget_connect (
  slate_widget_t *                      pSource,
  slate_token_t                         signalName,
  slate_widget_t *                      pTarget,
  void const *                          pSlotFunc
);

int __slate_widget_insert (
  slate_widget_t *                      pParent,
  slate_widget_t *                      pChild
);

void __slate_widget_dirty (
  slate_widget_t *                      pWidget
);

#define slate_widget_dirty(pWidget) __slate_widget_dirty((slate_widget_t *)pWidget)

#define slate_widget_init(pWidget, pAllocator, type)                            \
__slate_widget_init((slate_widget_t *)pWidget, pAllocator, type, sizeof(*pWidget))

#define slate_widget_connect(source, token, target, slot)                       \
__slate_widget_connect((slate_widget_t *)source, token, (slate_widget_t *)target, (void const *)slot)

#define slate_widget_insert(parent, child)                                      \
__slate_widget_insert((slate_widget_t *)parent, (slate_widget_t *)child)

#define slate_widget_find_signal(pWidget, token)                                \
__slate_widget_find_signal((slate_widget_t *)pWidget, token)

#define slate_widget_add_signal(pWidget, token, pSignal)                        \
__slate_widget_add_signal((slate_widget_t *)pWidget, token, pSignal)

#define slate_widget_add_connect(pWidget, token, slot)                          \
__slate_widget_add_connect((slate_widget_t *)pWidget, token, (void const *)slot)

// slate_application_t

int slate_create_application (
  slate_application_info_t const *      pCreateInfo,
  slate_application_t **                pApplication
);

int slate_destroy_application (
  slate_application_t *                 pApplication
);

int slate_run_application (
  slate_application_t *                 pApplication
);

int slate_application_set_status_bar (
  slate_application_t *                 pApplication,
  slate_status_bar_t *                  pStatusBar
);

// slate_status_bar_t

int slate_create_status_bar (
  slate_alloc_t const *                 pAllocator,
  slate_status_bar_t **                 pStatusBar
);

// slate_label_t

int slate_create_label (
  slate_alloc_t const *                 pAllocator,
  slate_label_t **                      pLabel
);

int slate_label_set_align (
  slate_label_t *                       pLabel,
  slate_align_t                         alignment
);

int slate_label_set_text (
  slate_label_t *                       pLabel,
  char const *                          text
);

int slate_label_set_text_n (
  slate_label_t *                       pLabel,
  char const *                          text,
  size_t                                n
);

#define slate_label_set_text_k(pLabel, text) slate_label_set_text_n(pLabel, text, sizeof(text) - 1)

#ifdef    __cplusplus
}
#endif // __cplusplus
#endif // SLATE_H
