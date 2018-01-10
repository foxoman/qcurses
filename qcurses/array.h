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
#ifndef   QCURSES_ARRAY_H
#define   QCURSES_ARRAY_H

#include "qcurses.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Array Defines
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_ARRAY(type) struct {                                     \
  type *                                pData;                                  \
  uint32_t                              count;                                  \
  uint32_t                              capacity;                               \
}

////////////////////////////////////////////////////////////////////////////////
// Array Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
typedef QCURSES_DEFINE_ARRAY(char) qcurses_array_t;

////////////////////////////////////////////////////////////////////////////////
// Array Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_array_init(
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  size_t                                elementSize,
  uint32_t                              initialCapacity
);

//------------------------------------------------------------------------------
#define qcurses_array_init(pAllocator, pArray, initialCapacity)                 \
  __qcurses_array_init(                                                         \
    pAllocator,                                                                 \
    (qcurses_array_t *)(pArray),                                                \
    sizeof((pArray)->pData[0]),                                                 \
    initialCapacity                                                             \
  )

//------------------------------------------------------------------------------
void QCURSESCALL __qcurses_array_deinit (
  qcurses_alloc_t const *                 pAllocator,
  qcurses_array_t *                       pArray
);

//------------------------------------------------------------------------------
#define qcurses_array_deinit(pAllocator, pArray)                                \
  __qcurses_array_deinit(                                                       \
    pAllocator,                                                                 \
    (qcurses_array_t *)(pArray),                                                \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_array_resize(
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  size_t                                elementSize,
  uint32_t                              capacity
);

//------------------------------------------------------------------------------
#define qcurses_array_resize(pAllocator, pArray, newCapacity)                   \
  __qcurses_array_resize(                                                       \
    pAllocator,                                                                 \
    (qcurses_array_t *)(pArray),                                                \
    sizeof((pArray)->pData[0]),                                                 \
    newCapacity                                                                 \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_array_grow (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  uint32_t                              elementSize
);

//------------------------------------------------------------------------------
#define qcurses_array_grow(pAllocator, pArray)                                  \
  __qcurses_array_grow(                                                         \
    pAllocator,                                                                 \
    (qcurses_array_t *)(pArray),                                                \
    sizeof((pArray)->pData[0])                                                  \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_array_push (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  uint32_t                              elementSize,
  void const *                          pData
);

//------------------------------------------------------------------------------
#define qcurses_array_push(pAllocator, pArray, element)                         \
  __qcurses_array_push(                                                         \
    pAllocator,                                                                 \
    (qcurses_array_t *)(pArray),                                                \
    sizeof((pArray)->pData[0]),                                                 \
    &element                                                                    \
  )

//------------------------------------------------------------------------------
#define qcurses_array_clear(pArray)                                             \
  ((pArray)->count = 0)

//------------------------------------------------------------------------------
#define qcurses_array_full(pArray)                                              \
  ((pArray)->count == (pArray)->capacity)

//------------------------------------------------------------------------------
#define qcurses_array_empty(pArray)                                             \
  ((pArray)->count == 0)

//------------------------------------------------------------------------------
#define qcurses_array_ensure(pAllocator, pArray)                                \
  ((qcurses_array_full(pArray)) ? qcurses_array_grow(pAllocator, pArray) : 0)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_ARRAY_H
