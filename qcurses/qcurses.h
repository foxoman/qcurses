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
#ifndef   QCURSES_H
#define   QCURSES_H

#include <stdint.h> // integral typedefs
#include <stdlib.h> // size_t typedef
#include <errno.h>  // Error codes

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// QCurses Defines
////////////////////////////////////////////////////////////////////////////////

// Standard defines
#define QFALSE 0
#define QTRUE  1
#define QBOOL(stmt) ((stmt) ? QTRUE : QFALSE)
#define QINFINITE            SIZE_MAX
#define QSIGNAL_MAX          UINT32_MAX
#define QPIMPL_NAME(name)    _##name##_impl_t
#define QSLOT_NAME(name)     _##name##_slot_t
#define QRECALC_NAME(name)   _##name##_recalc_t
#define QPAINTER_NAME(name)  _##name##_painter_t
#define QPIMPL_STRUCT(name)  struct QPIMPL_NAME(name)
#define QMIN(a,b)            (((a) < (b)) ? (a) : (b))
#define QMAX(a,b)            (((a) > (b)) ? (a) : (b))

// Forward-declaration defines
#define QDECLARE_STRUCT(name)    typedef struct name name
#define QDECLARE_ENUM(name)      typedef enum name name
#define QDECLARE_FLAG_TYPE(name) typedef qflags_t name
#define QDECLARE_FLAG_BITS(name) typedef enum name name
#define QDECLARE_FLAGS(bitsName, flagName)                               \
  QDECLARE_FLAG_BITS(bitsName); QDECLARE_FLAG_TYPE(flagName)

// Implementation defines
#define QCURSESCALL
#define QCURSESPTR

////////////////////////////////////////////////////////////////////////////////
// QCurses Enumerations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
enum qkey_t {
  QKEY_INVALID = 0x00,
  QKEY_TAB = 0x09,
  QKEY_RETURN = 0x0D,
  QKEY_ESCAPE = 0x1B,
  QKEY_SPACE = 0x20,
  QKEY_SINGLE_QUOTE = 0x27,
  QKEY_COMMA = 0x2C,
  QKEY_MINUS = 0x2D,
  QKEY_PERIOD = 0x2E,
  QKEY_SOLIDUS = 0x2F,
  QKEY_0 = '0',
  QKEY_1 = '1',
  QKEY_2 = '2',
  QKEY_3 = '3',
  QKEY_4 = '4',
  QKEY_5 = '5',
  QKEY_6 = '6',
  QKEY_7 = '7',
  QKEY_8 = '8',
  QKEY_9 = '9',
  QKEY_SEMICOLON = 0x3B,
  QKEY_EQUALS = 0x3D,
  QKEY_A = 'a',
  QKEY_B = 'b',
  QKEY_C = 'c',
  QKEY_D = 'd',
  QKEY_E = 'e',
  QKEY_F = 'f',
  QKEY_G = 'g',
  QKEY_H = 'h',
  QKEY_I = 'i',
  QKEY_J = 'j',
  QKEY_K = 'k',
  QKEY_L = 'l',
  QKEY_M = 'm',
  QKEY_N = 'n',
  QKEY_O = 'o',
  QKEY_P = 'p',
  QKEY_Q = 'q',
  QKEY_R = 'r',
  QKEY_S = 's',
  QKEY_T = 't',
  QKEY_U = 'u',
  QKEY_V = 'v',
  QKEY_W = 'w',
  QKEY_X = 'x',
  QKEY_Y = 'y',
  QKEY_Z = 'z',
  QKEY_LEFT_BRACKET = 0x5B,
  QKEY_RIGHT_BRACKET = 0x5D,
  QKEY_REVERSE_SOLIDUS = 0x5C,
  QKEY_BACKTICK = 0x60,
  QKEY_BACKSPACE = 0x7F,

  // Escape Sequences:
  QKEY_INSERT = 0x100,
  QKEY_DELETE,
  QKEY_PAGE_UP,
  QKEY_PAGE_DOWN,
  QKEY_LEFT,
  QKEY_RIGHT,
  QKEY_UP,
  QKEY_DOWN,
  QKEY_END,
  QKEY_HOME,

  // Function keys
  QKEY_F0,
  QKEY_F1,
  QKEY_F2,
  QKEY_F3,
  QKEY_F4,
  QKEY_F5,
  QKEY_F6,
  QKEY_F7,
  QKEY_F8,
  QKEY_F9,
  QKEY_F10,
  QKEY_F11,
  QKEY_F12,

  // TODO: Add support.
  QKEY_NUM_0,
  QKEY_NUM_1,
  QKEY_NUM_2,
  QKEY_NUM_3,
  QKEY_NUM_4,
  QKEY_NUM_5,
  QKEY_NUM_6,
  QKEY_NUM_7,
  QKEY_NUM_8,
  QKEY_NUM_9,
};

//------------------------------------------------------------------------------
#define QMOUSE_STATE(n, state) (state << ((n - 1) * QMOUSE_BITS))
#define QMOUSE_BITS 2
#define QMOUSE_PRESSED_BIT 0x01
#define QMOUSE_RELEASED_BIT 0x02
enum qmouse_bits_t {
  QMOUSE_LEFT_PRESSED_BIT = QMOUSE_STATE(1, QMOUSE_PRESSED_BIT),
  QMOUSE_LEFT_RELEASED_BIT = QMOUSE_STATE(1, QMOUSE_RELEASED_BIT),
  QMOUSE_MIDDLE_PRESSED_BIT = QMOUSE_STATE(2, QMOUSE_PRESSED_BIT),
  QMOUSE_MIDDLE_RELEASED_BIT = QMOUSE_STATE(2, QMOUSE_RELEASED_BIT),
  QMOUSE_RIGHT_PRESSED_BIT = QMOUSE_STATE(3, QMOUSE_PRESSED_BIT),
  QMOUSE_RIGHT_RELEASED_BIT = QMOUSE_STATE(3, QMOUSE_RELEASED_BIT),
};

//------------------------------------------------------------------------------
enum qstate_bits_t {
  QSTATE_DIRTY_BIT = 0x01,
  QSTATE_ENABLED_BIT = 0x02,
  QSTATE_VISIBLE_BIT = 0x04,
};

//------------------------------------------------------------------------------
enum qmodifier_bits_t {
  QMODIFIER_SHIFT_BIT = 0x01,
  QMODIFIER_CONTROL_BIT = 0x02,
};
#define QMODIFIER_NONE 0

//------------------------------------------------------------------------------
enum qpolicy_bits_t {
  QPOLICY_GROW_BIT = 0x01,
  QPOLICY_SHRINK_BIT = 0x02,
  QPOLICY_EXPAND_BIT = 0x04,
  QPOLICY_IGNORE_BIT = 0x08
};
#define QPOLICY_FIXED      (0)
#define QPOLICY_MINIMUM    (QPOLICY_GROW_BIT)
#define QPOLICY_MAXIMUM    (QPOLICY_SHRINK_BIT)
#define QPOLICY_PREFERRED  (QPOLICY_GROW_BIT | QPOLICY_SHRINK_BIT)
#define QPOLICY_EXPANDING  (QPOLICY_GROW_BIT | QPOLICY_SHRINK_BIT | QPOLICY_EXPAND_BIT)
#define QPOLICY_FLEXIBLE   (QPOLICY_GROW_BIT | QPOLICY_EXPAND_BIT)
#define QPOLICY_IGNORED    (QPOLICY_GROW_BIT | QPOLICY_SHRINK_BIT | QPOLICY_IGNORE_BIT)

//------------------------------------------------------------------------------
enum qalign_bits_t {
  // Mutually-exclusive horizontal bits:
  QALIGN_LEFT_BIT = 0x0001,
  QALIGN_CENTER_BIT = 0x0002,
  QALIGN_RIGHT_BIT = 0x0003,
  // Mutually-exclusive vertical bits:
  QALIGN_TOP_BIT = 0x0010,
  QALIGN_MIDDLE_BIT = 0x0020,
  QALIGN_BOTTOM_BIT = 0x0030,
};
#define QALIGN_HORIZONTAL_MASK 0x000F
#define QALIGN_VERTICAL_MASK   0x00F0

////////////////////////////////////////////////////////////////////////////////
// QCurses Declarations
////////////////////////////////////////////////////////////////////////////////

// Standard Types
typedef int       qbool_t;
typedef uint32_t  qflags_t;

// Enums
QDECLARE_ENUM(qkey_t);
QDECLARE_ENUM(qlayout_format_t);

// Flags
QDECLARE_FLAGS(qalign_bits_t, qalign_t);
QDECLARE_FLAGS(qmodifier_bits_t, qmodifier_t);
QDECLARE_FLAGS(qmouse_bits_t, qmouse_t);
QDECLARE_FLAGS(qpolicy_bits_t, qpolicy_t);
QDECLARE_FLAGS(qstate_bits_t, qstate_t);

// Structs
QDECLARE_STRUCT(qalloc_t);
QDECLARE_STRUCT(qapplication_t);
QDECLARE_STRUCT(qapplication_info_t);
QDECLARE_STRUCT(qbounds_t);
QDECLARE_STRUCT(qcoord_t);
QDECLARE_STRUCT(qlayout_t);
QDECLARE_STRUCT(qpainter_t);
QDECLARE_STRUCT(qregion_t);
QDECLARE_STRUCT(qslot_t);
QDECLARE_STRUCT(qwidget_t);
QDECLARE_STRUCT(qwidget_config_t);

////////////////////////////////////////////////////////////////////////////////
// QCurses Structures
////////////////////////////////////////////////////////////////////////////////

struct qalloc_t;
typedef void* (QCURSESPTR *qalloc_pfn)(struct qalloc_t const * pData, size_t n, size_t align);
typedef void* (QCURSESPTR *qrealloc_pfn)(struct qalloc_t const * pData, void * ptr, size_t n);
typedef void  (QCURSESPTR *qfree_pfn)(struct qalloc_t const * pData, void * ptr);

//------------------------------------------------------------------------------
struct qalloc_t {
  qalloc_pfn                     pfnAllocate;
  qrealloc_pfn                   pfnReallocate;
  qfree_pfn                      pfnFree;
};

////////////////////////////////////////////////////////////////////////////////
// QCurses Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void * QCURSESCALL qallocate_unsafe (
  qalloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
);

//------------------------------------------------------------------------------
void * QCURSESCALL qreallocate_unsafe (
  qalloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
);

//------------------------------------------------------------------------------
void QCURSESCALL qfree_unsafe (
  qalloc_t const *               pAllocator,
  void *                                ptr
);

//------------------------------------------------------------------------------
void * QCURSESCALL qallocate (
  qalloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
);

//------------------------------------------------------------------------------
void * QCURSESCALL qreallocate (
  qalloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
);

//------------------------------------------------------------------------------
void QCURSESCALL qfree (
  qalloc_t const *               pAllocator,
  void *                                ptr
);

//------------------------------------------------------------------------------
void QCURSESCALL qhost_allocator_init (
  qalloc_t *                     pAllocator
);

//------------------------------------------------------------------------------
qalloc_t const * QCURSESCALL qdefault_allocator ();

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_H
