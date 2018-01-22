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
 ******************************************************************************/
#ifndef   QCURSES_QCURSES_H
#define   QCURSES_QCURSES_H

#include "fwdqcurses.h"
#include <errno.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// QCurses Defines
////////////////////////////////////////////////////////////////////////////////

#define QCURSES_FALSE 0
#define QCURSES_TRUE  1
#define QCURSES_BOOL(stmt) ((stmt) ? QCURSES_TRUE : QCURSES_FALSE)
#define QCURSES_INFINITE            SIZE_MAX
#define QCURSES_SIGNAL_MAX          UINT32_MAX
#define QCURSES_PIMPL_NAME(name)    _##name##_impl_t
#define QCURSES_SLOT_NAME(name)     _##name##_slot_t
#define QCURSES_RECALC_NAME(name)   _##name##_recalc_t
#define QCURSES_PAINTER_NAME(name)  _##name##_painter_t
#define QCURSES_PIMPL_STRUCT(name)  struct QCURSES_PIMPL_NAME(name)
#define QCURSES_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define QCURSES_MAX(a,b)            (((a) > (b)) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////
// QCurses Enumerations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
enum qcurses_keycode_t {
  QCURSES_KEYCODE_INVALID = 0x00,
  QCURSES_KEYCODE_TAB = 0x09,
  QCURSES_KEYCODE_RETURN = 0x0D,
  QCURSES_KEYCODE_ESCAPE = 0x1B,
  QCURSES_KEYCODE_SPACE = 0x20,
  QCURSES_KEYCODE_SINGLE_QUOTE = 0x27,
  QCURSES_KEYCODE_COMMA = 0x2C,
  QCURSES_KEYCODE_MINUS = 0x2D,
  QCURSES_KEYCODE_PERIOD = 0x2E,
  QCURSES_KEYCODE_SOLIDUS = 0x2F,
  QCURSES_KEYCODE_0 = '0',
  QCURSES_KEYCODE_1 = '1',
  QCURSES_KEYCODE_2 = '2',
  QCURSES_KEYCODE_3 = '3',
  QCURSES_KEYCODE_4 = '4',
  QCURSES_KEYCODE_5 = '5',
  QCURSES_KEYCODE_6 = '6',
  QCURSES_KEYCODE_7 = '7',
  QCURSES_KEYCODE_8 = '8',
  QCURSES_KEYCODE_9 = '9',
  QCURSES_KEYCODE_SEMICOLON = 0x3B,
  QCURSES_KEYCODE_EQUALS = 0x3D,
  QCURSES_KEYCODE_A = 'a',
  QCURSES_KEYCODE_B = 'b',
  QCURSES_KEYCODE_C = 'c',
  QCURSES_KEYCODE_D = 'd',
  QCURSES_KEYCODE_E = 'e',
  QCURSES_KEYCODE_F = 'f',
  QCURSES_KEYCODE_G = 'g',
  QCURSES_KEYCODE_H = 'h',
  QCURSES_KEYCODE_I = 'i',
  QCURSES_KEYCODE_J = 'j',
  QCURSES_KEYCODE_K = 'k',
  QCURSES_KEYCODE_L = 'l',
  QCURSES_KEYCODE_M = 'm',
  QCURSES_KEYCODE_N = 'n',
  QCURSES_KEYCODE_O = 'o',
  QCURSES_KEYCODE_P = 'p',
  QCURSES_KEYCODE_Q = 'q',
  QCURSES_KEYCODE_R = 'r',
  QCURSES_KEYCODE_S = 's',
  QCURSES_KEYCODE_T = 't',
  QCURSES_KEYCODE_U = 'u',
  QCURSES_KEYCODE_V = 'v',
  QCURSES_KEYCODE_W = 'w',
  QCURSES_KEYCODE_X = 'x',
  QCURSES_KEYCODE_Y = 'y',
  QCURSES_KEYCODE_Z = 'z',
  QCURSES_KEYCODE_LEFT_BRACKET = 0x5B,
  QCURSES_KEYCODE_RIGHT_BRACKET = 0x5D,
  QCURSES_KEYCODE_REVERSE_SOLIDUS = 0x5C,
  QCURSES_KEYCODE_BACKTICK = 0x60,
  QCURSES_KEYCODE_BACKSPACE = 0x7F,

  // Escape Sequences:
  // TODO: Partial support.
  QCURSES_KEYCODE_INSERT = 0x100,
  QCURSES_KEYCODE_DELETE,
  QCURSES_KEYCODE_PAGE_UP,
  QCURSES_KEYCODE_PAGE_DOWN,
  QCURSES_KEYCODE_LEFT,
  QCURSES_KEYCODE_RIGHT,
  QCURSES_KEYCODE_UP,
  QCURSES_KEYCODE_DOWN,
  QCURSES_KEYCODE_END,
  QCURSES_KEYCODE_HOME,

  // Function keys
  QCURSES_KEYCODE_F0,
  QCURSES_KEYCODE_F1,
  QCURSES_KEYCODE_F2,
  QCURSES_KEYCODE_F3,
  QCURSES_KEYCODE_F4,
  QCURSES_KEYCODE_F5,
  QCURSES_KEYCODE_F6,
  QCURSES_KEYCODE_F7,
  QCURSES_KEYCODE_F8,
  QCURSES_KEYCODE_F9,
  QCURSES_KEYCODE_F10,
  QCURSES_KEYCODE_F11,
  QCURSES_KEYCODE_F12,

  // TODO: Add support.
  QCURSES_KEYCODE_NUM_0,
  QCURSES_KEYCODE_NUM_1,
  QCURSES_KEYCODE_NUM_2,
  QCURSES_KEYCODE_NUM_3,
  QCURSES_KEYCODE_NUM_4,
  QCURSES_KEYCODE_NUM_5,
  QCURSES_KEYCODE_NUM_6,
  QCURSES_KEYCODE_NUM_7,
  QCURSES_KEYCODE_NUM_8,
  QCURSES_KEYCODE_NUM_9,
};

//------------------------------------------------------------------------------
#define QCURSES_MOUSE_STATE(n, state) (state << ((n - 1) * QCURSES_MOUSE_BUTTON_BITS))
#define QCURSES_MOUSE_BUTTON_BITS 2
#define QCURSES_MOUSE_BUTTON_PRESSED_BIT 0x01
#define QCURSES_MOUSE_BUTTON_RELEASED_BIT 0x02
enum qcurses_mouse_bits_t {
  QCURSES_MOUSE_BUTTON_LEFT_PRESSED_BIT = QCURSES_MOUSE_STATE(1, QCURSES_MOUSE_BUTTON_PRESSED_BIT),
  QCURSES_MOUSE_BUTTON_LEFT_RELEASED_BIT = QCURSES_MOUSE_STATE(1, QCURSES_MOUSE_BUTTON_RELEASED_BIT),
  QCURSES_MOUSE_BUTTON_MIDDLE_PRESSED_BIT = QCURSES_MOUSE_STATE(2, QCURSES_MOUSE_BUTTON_PRESSED_BIT),
  QCURSES_MOUSE_BUTTON_MIDDLE_RELEASED_BIT = QCURSES_MOUSE_STATE(2, QCURSES_MOUSE_BUTTON_RELEASED_BIT),
  QCURSES_MOUSE_BUTTON_RIGHT_PRESSED_BIT = QCURSES_MOUSE_STATE(3, QCURSES_MOUSE_BUTTON_PRESSED_BIT),
  QCURSES_MOUSE_BUTTON_RIGHT_RELEASED_BIT = QCURSES_MOUSE_STATE(3, QCURSES_MOUSE_BUTTON_RELEASED_BIT),
};

//------------------------------------------------------------------------------
enum qcurses_state_bits_t {
  QCURSES_STATE_DIRTY_BIT = 0x01,
  QCURSES_STATE_PARENT_BIT = 0x02,
  QCURSES_STATE_VISIBLE_BIT = 0x04,
};

//------------------------------------------------------------------------------
enum qcurses_modifier_bits_t {
  QCURSES_MODIFIER_SHIFT_BIT = 0x01,
  QCURSES_MODIFIER_CONTROL_BIT = 0x02,
};
#define QCURSES_MODIFIER_NONE 0

//------------------------------------------------------------------------------
enum qcurses_policy_bits_t {
  QCURSES_POLICY_GROW_BIT = 0x01,
  QCURSES_POLICY_SHRINK_BIT = 0x02,
  QCURSES_POLICY_EXPAND_BIT = 0x04,
  QCURSES_POLICY_IGNORE_BIT = 0x08
};
#define QCURSES_POLICY_FIXED      (0)
#define QCURSES_POLICY_MINIMUM    (QCURSES_POLICY_GROW_BIT)
#define QCURSES_POLICY_MAXIMUM    (QCURSES_POLICY_SHRINK_BIT)
#define QCURSES_POLICY_PREFERRED  (QCURSES_POLICY_GROW_BIT | QCURSES_POLICY_SHRINK_BIT)
#define QCURSES_POLICY_EXPANDING  (QCURSES_POLICY_GROW_BIT | QCURSES_POLICY_SHRINK_BIT | QCURSES_POLICY_EXPAND_BIT)
#define QCURSES_POLICY_FLEXIBLE   (QCURSES_POLICY_GROW_BIT | QCURSES_POLICY_EXPAND_BIT)
#define QCURSES_POLICY_IGNORED    (QCURSES_POLICY_GROW_BIT | QCURSES_POLICY_SHRINK_BIT | QCURSES_POLICY_IGNORE_BIT)

//------------------------------------------------------------------------------
enum qcurses_align_bits_t {
  // Mutually-exclusive horizontal bits:
  QCURSES_ALIGN_LEFT_BIT = 0x0001,
  QCURSES_ALIGN_CENTER_BIT = 0x0002,
  QCURSES_ALIGN_RIGHT_BIT = 0x0003,
  // Mutually-exclusive vertical bits:
  QCURSES_ALIGN_TOP_BIT = 0x0010,
  QCURSES_ALIGN_MIDDLE_BIT = 0x0020,
  QCURSES_ALIGN_BOTTOM_BIT = 0x0030,
};
#define QCURSES_ALIGN_HORIZONTAL_MASK 0x000F
#define QCURSES_ALIGN_VERTICAL_MASK   0x00F0

// TODO: This should go elsewhere.
//------------------------------------------------------------------------------
#define P(pointer) ((pointer)->pImpl)
#define W(pointer) ((qcurses_widget_t *)(pointer))

////////////////////////////////////////////////////////////////////////////////
// QCurses Structures
////////////////////////////////////////////////////////////////////////////////

struct qcurses_alloc_t;
typedef void* (QCURSESPTR *qcurses_alloc_pfn)(struct qcurses_alloc_t const * pData, size_t n, size_t align);
typedef void* (QCURSESPTR *qcurses_realloc_pfn)(struct qcurses_alloc_t const * pData, void * ptr, size_t n);
typedef void  (QCURSESPTR *qcurses_free_pfn)(struct qcurses_alloc_t const * pData, void * ptr);

//------------------------------------------------------------------------------
struct qcurses_alloc_t {
  qcurses_alloc_pfn                     pfnAllocate;
  qcurses_realloc_pfn                   pfnReallocate;
  qcurses_free_pfn                      pfnFree;
};

////////////////////////////////////////////////////////////////////////////////
// QCurses Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void * qcurses_allocate_unsafe (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
);

//------------------------------------------------------------------------------
void * qcurses_reallocate_unsafe (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
);

//------------------------------------------------------------------------------
void qcurses_free_unsafe (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
);

//------------------------------------------------------------------------------
void * qcurses_allocate (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
);

//------------------------------------------------------------------------------
void * qcurses_reallocate (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
);

//------------------------------------------------------------------------------
void qcurses_free (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
);

//------------------------------------------------------------------------------
void qcurses_host_allocator_init (
  qcurses_alloc_t *                     pAllocator
);

//------------------------------------------------------------------------------
qcurses_alloc_t const * qcurses_default_allocator ();

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_QCURSES_H
