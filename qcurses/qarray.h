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
#ifndef   QARRAY_H
#define   QARRAY_H

#include "qcurses.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Array Defines
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define QDEFINE_ARRAY(type) struct {                                            \
  type *                                pData;                                  \
  uint32_t                              count;                                  \
  uint32_t                              capacity;                               \
}

////////////////////////////////////////////////////////////////////////////////
// Array Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
typedef QDEFINE_ARRAY(char) qarray_t;

////////////////////////////////////////////////////////////////////////////////
// Array Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_init(
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  size_t                                elementSize,
  uint32_t                              initialCapacity
);

//------------------------------------------------------------------------------
#define qarray_init(pAllocator, pArray, initialCapacity)                        \
  __qarray_init(                                                                \
    pAllocator,                                                                 \
    (qarray_t *)(pArray),                                                       \
    sizeof((pArray)->pData[0]),                                                 \
    initialCapacity                                                             \
  )

//------------------------------------------------------------------------------
void QCURSESCALL __qarray_deinit (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray
);

//------------------------------------------------------------------------------
#define qarray_deinit(pAllocator, pArray)                                       \
  __qarray_deinit(                                                              \
    pAllocator,                                                                 \
    (qarray_t *)(pArray),                                                       \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_resize(
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  size_t                                elementSize,
  uint32_t                              capacity
);

//------------------------------------------------------------------------------
#define qarray_resize(pAllocator, pArray, newCapacity)                          \
  __qarray_resize(                                                              \
    pAllocator,                                                                 \
    (qarray_t *)(pArray),                                                       \
    sizeof((pArray)->pData[0]),                                                 \
    newCapacity                                                                 \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_grow (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  uint32_t                              elementSize
);

//------------------------------------------------------------------------------
#define qarray_grow(pAllocator, pArray)                                         \
  __qarray_grow(                                                                \
    pAllocator,                                                                 \
    (qarray_t *)(pArray),                                                       \
    sizeof((pArray)->pData[0])                                                  \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_push (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  uint32_t                              elementSize,
  void const *                          pData
);

//------------------------------------------------------------------------------
#define qarray_push(pAllocator, pArray, element)                                \
  __qarray_push(                                                                \
    pAllocator,                                                                 \
    (qarray_t *)(pArray),                                                       \
    sizeof((pArray)->pData[0]),                                                 \
    &element                                                                    \
  )

//------------------------------------------------------------------------------
#define qarray_clear(pArray)                                                    \
  ((pArray)->count = 0)

//------------------------------------------------------------------------------
#define qarray_full(pArray)                                                     \
  ((pArray)->count == (pArray)->capacity)

//------------------------------------------------------------------------------
#define qarray_empty(pArray)                                                    \
  ((pArray)->count == 0)

//------------------------------------------------------------------------------
#define qarray_ensure(pAllocator, pArray)                                       \
  ((qarray_full(pArray)) ? qarray_grow(pAllocator, pArray) : 0)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QARRAY_H
