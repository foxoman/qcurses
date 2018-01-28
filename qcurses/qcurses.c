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

#include "qcurses.h"
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ncurses.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// QCurses Static Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static void * qhost_allocate (
  qalloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  (void)pAllocator;
  return aligned_alloc(align, n);
}

//------------------------------------------------------------------------------
static void * qhost_reallocate (
  qalloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  (void)pAllocator;
  return realloc(ptr, n);
}

//------------------------------------------------------------------------------
static void qhost_free (
  qalloc_t const *               pAllocator,
  void *                                ptr
) {
  (void)pAllocator;
  free(ptr);
}

//------------------------------------------------------------------------------
static qalloc_t const sDefaultAllocator = {
  &qhost_allocate,
  &qhost_reallocate,
  &qhost_free
};

////////////////////////////////////////////////////////////////////////////////
// QCurses Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void * QCURSESCALL qallocate_unsafe (
  qalloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  return pAllocator->pfnAllocate(pAllocator, n, align);
}

//------------------------------------------------------------------------------
void * QCURSESCALL qreallocate_unsafe (
  qalloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  return pAllocator->pfnReallocate(pAllocator, ptr, n);
}

//------------------------------------------------------------------------------
void QCURSESCALL qfree_unsafe (
  qalloc_t const *               pAllocator,
  void *                                ptr
) {
  pAllocator->pfnFree(pAllocator, ptr);
}

//------------------------------------------------------------------------------
void * QCURSESCALL qallocate (
  qalloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  if (!pAllocator)
    return qhost_allocate(pAllocator, n, align);
  return pAllocator->pfnAllocate(pAllocator, n, align);
}

//------------------------------------------------------------------------------
void * QCURSESCALL qreallocate (
  qalloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  if (!pAllocator)
    return qhost_reallocate(pAllocator, ptr, n);
  return pAllocator->pfnReallocate(pAllocator, ptr, n);
}

//------------------------------------------------------------------------------
void QCURSESCALL qfree (
  qalloc_t const *               pAllocator,
  void *                                ptr
) {
  if (!pAllocator)
    qhost_free(pAllocator, ptr);
  else
    pAllocator->pfnFree(pAllocator, ptr);
}

//------------------------------------------------------------------------------
void QCURSESCALL qhost_allocator_init (
  qalloc_t *                     pAllocator
) {
  pAllocator->pfnAllocate = &qhost_allocate;
  pAllocator->pfnReallocate = &qhost_reallocate;
  pAllocator->pfnFree = &qhost_free;
}

//------------------------------------------------------------------------------
qalloc_t const * QCURSESCALL qdefault_allocator () {
  return &sDefaultAllocator;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
