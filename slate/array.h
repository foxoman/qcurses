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
#ifndef   SLATE_ARRAY_H
#define   SLATE_ARRAY_H

#include "slate.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Defines
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define SLATE_DEFINE_ARRAY(type) struct {                                       \
  type *                                pData;                                  \
  uint32_t                              count;                                  \
  uint32_t                              capacity;                               \
}

////////////////////////////////////////////////////////////////////////////////
// Slate Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
typedef SLATE_DEFINE_ARRAY(char)                 slate_array_t;

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int SLATECALL __slate_array_init(
  slate_alloc_t const *                 pAllocator,
  slate_array_t *                       pArray,
  size_t                                elementSize,
  uint32_t                              initialCapacity
);

//------------------------------------------------------------------------------
#define slate_array_init(pAllocator, pArray, initialCapacity)                   \
  __slate_array_init(                                                           \
    pAllocator,                                                                 \
    (slate_array_t *)(pArray),                                                  \
    sizeof((pArray)->pData[0]),                                                 \
    initialCapacity                                                             \
  )

//------------------------------------------------------------------------------
void SLATECALL __slate_array_deinit (
  slate_alloc_t const *                 pAllocator,
  slate_array_t *                       pArray
);

//------------------------------------------------------------------------------
#define slate_array_deinit(pAllocator, pArray)                                  \
  __slate_array_deinit(                                                         \
    pAllocator,                                                                 \
    (slate_array_t *)(pArray),                                                  \
  )

//------------------------------------------------------------------------------
int SLATECALL __slate_array_resize(
  slate_alloc_t const *                 pAllocator,
  slate_array_t *                       pArray,
  size_t                                elementSize,
  uint32_t                              capacity
);

//------------------------------------------------------------------------------
#define slate_array_resize(pAllocator, pArray, newCapacity)                     \
  __slate_array_resize(                                                         \
    pAllocator,                                                                 \
    (slate_array_t *)(pArray),                                                  \
    sizeof((pArray)->pData[0]),                                                 \
    newCapacity                                                                 \
  )

//------------------------------------------------------------------------------
int SLATECALL __slate_array_grow (
  slate_alloc_t const *                 pAllocator,
  slate_array_t *                       pArray,
  uint32_t                              elementSize
);

//------------------------------------------------------------------------------
#define slate_array_grow(pAllocator, pArray)                                    \
  __slate_array_grow(                                                           \
    pAllocator,                                                                 \
    (slate_array_t *)(pArray),                                                  \
    sizeof((pArray)->pData[0])                                                  \
  )

//------------------------------------------------------------------------------
int SLATECALL __slate_array_push (
  slate_alloc_t const *                 pAllocator,
  slate_array_t *                       pArray,
  uint32_t                              elementSize,
  void const *                          pData
);

//------------------------------------------------------------------------------
#define slate_array_push(pAllocator, pArray, element)                           \
  __slate_array_push(                                                           \
    pAllocator,                                                                 \
    (slate_array_t *)(pArray),                                                  \
    sizeof((pArray)->pData[0]),                                                 \
    &element                                                                    \
  )

//------------------------------------------------------------------------------
#define slate_array_clear(pArray)                                               \
  ((pArray)->count = 0)

//------------------------------------------------------------------------------
#define slate_array_full(pArray)                                                \
  ((pArray)->count == (pArray)->capacity)

//------------------------------------------------------------------------------
#define slate_array_empty(pArray)                                               \
  ((pArray)->count == 0)

//------------------------------------------------------------------------------
#define slate_array_ensure(pAllocator, pArray)                                  \
  ((slate_array_full(pArray)) ? slate_array_grow(pAllocator, pArray) : 0)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_ARRAY_H
