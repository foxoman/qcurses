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
#ifndef   SLATE_SLATE_H
#define   SLATE_SLATE_H

#include "fwdslate.h"
#include <errno.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Defines
////////////////////////////////////////////////////////////////////////////////

#define SLATECALL
#define SLATEPTR
#define SLATE_FALSE 0
#define SLATE_TRUE  1
#define SLATE_BOOL(stmt) ((stmt) ? SLATE_TRUE : SLATE_FALSE)
#define SLATE_INFINITE            SIZE_MAX
#define SLATE_SIGNAL_MAX          UINT32_MAX
#define SLATE_PIMPL_NAME(name)    _##name##_impl_t
#define SLATE_SLOT_NAME(name)     _##name##_slot_t
#define SLATE_PIMPL_STRUCT(name)  struct SLATE_PIMPL_NAME(name)
#define SLATE_MIN(a,b)            (((a) < (b)) ? (a) : (b))
#define SLATE_MAX(a,b)            (((a) > (b)) ? (a) : (b))

////////////////////////////////////////////////////////////////////////////////
// Slate Enumerations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
enum slate_keycode_t {
  SLATE_KEYCODE_INVALID = 0x00,
  SLATE_KEYCODE_TAB = 0x09,
  SLATE_KEYCODE_RETURN = 0x0D,
  SLATE_KEYCODE_ESCAPE = 0x1B,
  SLATE_KEYCODE_SPACE = 0x20,
  SLATE_KEYCODE_SINGLE_QUOTE = 0x27,
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

  // Function keys
  SLATE_KEYCODE_F0,
  SLATE_KEYCODE_F1,
  SLATE_KEYCODE_F2,
  SLATE_KEYCODE_F3,
  SLATE_KEYCODE_F4,
  SLATE_KEYCODE_F5,
  SLATE_KEYCODE_F6,
  SLATE_KEYCODE_F7,
  SLATE_KEYCODE_F8,
  SLATE_KEYCODE_F9,
  SLATE_KEYCODE_F10,
  SLATE_KEYCODE_F11,
  SLATE_KEYCODE_F12,

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
};

//------------------------------------------------------------------------------
enum slate_state_bits_t {
  SLATE_STATE_DIRTY_BIT = 0x01,
  SLATE_STATE_PARENT_BIT = 0x02,
  SLATE_STATE_VISIBLE_BIT = 0x04,
};

//------------------------------------------------------------------------------
enum slate_modifier_bits_t {
  SLATE_MODIFIER_SHIFT_BIT = 0x01,
  SLATE_MODIFIER_CONTROL_BIT = 0x02,
};
#define SLATE_MODIFIER_NONE 0

//------------------------------------------------------------------------------
enum slate_policy_bits_t {
  SLATE_POLICY_GROW_BIT = 0x01,
  SLATE_POLICY_SHRINK_BIT = 0x02,
  SLATE_POLICY_EXPAND_BIT = 0x04,
  SLATE_POLICY_IGNORE_BIT = 0x08
};
#define SLATE_POLICY_FIXED      (0)
#define SLATE_POLICY_MINIMUM    (SLATE_POLICY_GROW_BIT)
#define SLATE_POLICY_MAXIMUM    (SLATE_POLICY_SHRINK_BIT)
#define SLATE_POLICY_PREFERRED  (SLATE_POLICY_GROW_BIT | SLATE_POLICY_SHRINK_BIT)
#define SLATE_POLICY_EXPANDING  (SLATE_POLICY_GROW_BIT | SLATE_POLICY_SHRINK_BIT | SLATE_POLICY_EXPAND_BIT)
#define SLATE_POLICY_FLEXIBLE   (SLATE_POLICY_GROW_BIT | SLATE_POLICY_EXPAND_BIT)
#define SLATE_POLICY_IGNORED    (SLATE_POLICY_GROW_BIT | SLATE_POLICY_SHRINK_BIT | SLATE_POLICY_IGNORE_BIT)

//------------------------------------------------------------------------------
enum slate_align_bits_t {
  // Mutually-exclusive horizontal bits:
  SLATE_ALIGN_LEFT_BIT = 0x0001,
  SLATE_ALIGN_CENTER_BIT = 0x0002,
  SLATE_ALIGN_RIGHT_BIT = 0x0003,
  // Mutually-exclusive vertical bits:
  SLATE_ALIGN_TOP_BIT = 0x0010,
  SLATE_ALIGN_MIDDLE_BIT = 0x0020,
  SLATE_ALIGN_BOTTOM_BIT = 0x0030,
};
#define SLATE_ALIGN_HORIZONTAL_MASK 0x000F
#define SLATE_ALIGN_VERTICAL_MASK   0x00F0

// TODO: This should go elsewhere.
//------------------------------------------------------------------------------
#define P(pointer) ((pointer)->pImpl)
#define W(pointer) ((slate_widget_t *)(pointer))

////////////////////////////////////////////////////////////////////////////////
// Slate Structures
////////////////////////////////////////////////////////////////////////////////

typedef void* (SLATEPTR *slate_alloc_pfn)(void * pData, size_t n, size_t align);
typedef void* (SLATEPTR *slate_realloc_pfn)(void * pData, void * ptr, size_t n);
typedef void  (SLATEPTR *slate_free_pfn)(void * pData, void * ptr);

//------------------------------------------------------------------------------
struct slate_alloc_t {
  slate_alloc_pfn                       pfnAllocate;
  slate_realloc_pfn                     pfnReallocate;
  slate_free_pfn                        pfnFree;
  void *                                pUserData;
};

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void * slate_allocate (
  slate_alloc_t const *                 pAllocator,
  size_t                                n,
  size_t                                align
);

//------------------------------------------------------------------------------
void * slate_reallocate (
  slate_alloc_t const *                 pAllocator,
  void *                                ptr,
  size_t                                n
);

//------------------------------------------------------------------------------
void slate_free (
  slate_alloc_t const *                 pAllocator,
  void *                                ptr
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_SLATE_H
