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

#include "qarray.h"
#include <string.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Array Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_init  (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  size_t                                elementSize,
  uint32_t                              initialCapacity
) {
  pArray->pData = NULL;
  pArray->count = 0;
  pArray->capacity = 0;
  if (initialCapacity != 0) {
    return __qarray_resize(pAllocator, pArray, elementSize, initialCapacity);
  }
  return 0;
}

//------------------------------------------------------------------------------
void QCURSESCALL __qarray_deinit (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray
) {
  qfree(pAllocator, pArray->pData);
}

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_resize (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  size_t                                elementSize,
  uint32_t                              capacity
) {
  void * ptr;
  ptr = qreallocate(pAllocator, pArray->pData, elementSize * capacity);
  if (!ptr) {
    return ENOMEM;
  }
  pArray->pData = ptr;
  pArray->capacity = capacity;
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_grow (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
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

  return __qarray_resize(pAllocator, pArray, elementSize, newCapacity);
}

//------------------------------------------------------------------------------
int QCURSESCALL __qarray_push (
  qalloc_t const *                      pAllocator,
  qarray_t *                            pArray,
  uint32_t                              elementSize,
  void const *                          pData
) {
  int err;
  if (qarray_full(pArray)) {
    err = __qarray_grow(pAllocator, pArray, elementSize);
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
