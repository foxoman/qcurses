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

#include "array.h"
#include <string.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Array Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int __qcurses_array_init  (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  size_t                                elementSize,
  uint32_t                              initialCapacity
) {
  pArray->pData = NULL;
  pArray->count = 0;
  pArray->capacity = 0;
  if (initialCapacity != 0) {
    return __qcurses_array_resize(pAllocator, pArray, elementSize, initialCapacity);
  }
  return 0;
}

//------------------------------------------------------------------------------
void __qcurses_array_deinit (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray
) {
  qcurses_free(pAllocator, pArray->pData);
}

//------------------------------------------------------------------------------
int __qcurses_array_resize (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  size_t                                elementSize,
  uint32_t                              capacity
) {
  void * ptr;
  ptr = qcurses_reallocate(pAllocator, pArray->pData, elementSize * capacity);
  if (!ptr) {
    return ENOMEM;
  }
  pArray->pData = ptr;
  pArray->capacity = capacity;
  return 0;
}

//------------------------------------------------------------------------------
int __qcurses_array_grow (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  uint32_t                              elementSize
) {
  uint32_t newCapacity;

  // Double our capacity, handle the empty vector case.
  newCapacity = 2 * pArray->capacity;
  if (newCapacity == 0) {
    newCapacity = 1;
  }

  // See if we need to range limit the capacity.
  if (newCapacity < pArray->capacity) {
    newCapacity = UINT32_MAX;
    if (newCapacity == pArray->capacity) {
      return ERANGE;
    }
  }

  return __qcurses_array_resize(pAllocator, pArray, elementSize, newCapacity);
}

//------------------------------------------------------------------------------
int __qcurses_array_push (
  qcurses_alloc_t const *               pAllocator,
  qcurses_array_t *                     pArray,
  uint32_t                              elementSize,
  void const *                          pData
) {
  int err;
  if (qcurses_array_full(pArray)) {
    err = __qcurses_array_grow(pAllocator, pArray, elementSize);
    if (err) {
      return err;
    }
  }
  memcpy(pArray->pData + elementSize * pArray->count, pData, elementSize);
  ++pArray->count;
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
