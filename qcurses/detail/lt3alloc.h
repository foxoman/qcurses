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
#ifndef   QLT3ALLOC_H
#define   QLT3ALLOC_H

#include "../qcurses.h"
#include <lt3/alloc.h>

////////////////////////////////////////////////////////////////////////////////
// Allocator Pass-Through Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
typedef struct qlt3alloc_t {
  lt3_alloc_t                           instance;
  qalloc_t const *                      pAllocator;
} qlt3alloc_t;

//------------------------------------------------------------------------------
static void * qlt3allocate (
  qlt3alloc_t const *                   pAllocator,
  size_t                                n,
  size_t                                align
) {
  return pAllocator->pAllocator->pfnAllocate(pAllocator->pAllocator, n, align);
}

//------------------------------------------------------------------------------
static void * qlt3reallocate (
  qlt3alloc_t const *                   pAllocator,
  void *                                ptr,
  size_t                                n
) {
  return pAllocator->pAllocator->pfnReallocate(pAllocator->pAllocator, ptr, n);
}

//------------------------------------------------------------------------------
static void qlt3free (
  qlt3alloc_t const *                   pAllocator,
  void *                                ptr
) {
  pAllocator->pAllocator->pfnFree(pAllocator->pAllocator, ptr);
}

//------------------------------------------------------------------------------
static inline void qlt3alloc_init (
  qalloc_t const *                      pFrom,
  qlt3alloc_t *                         pTo
) {
  pTo->pAllocator = pFrom;
  pTo->instance.pfnAlloc = (lt3_pfn_alloc)qlt3allocate;
  pTo->instance.pfnRealloc = (lt3_pfn_realloc)qlt3reallocate;
  pTo->instance.pfnFree = (lt3_pfn_free)qlt3free;
}

#endif // QLT3ALLOC_H
