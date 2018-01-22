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
static void * qcurses_host_allocate (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  (void)pAllocator;
  return aligned_alloc(align, n);
}

//------------------------------------------------------------------------------
static void * qcurses_host_reallocate (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  (void)pAllocator;
  return realloc(ptr, n);
}

//------------------------------------------------------------------------------
static void qcurses_host_free (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
) {
  (void)pAllocator;
  free(ptr);
}

//------------------------------------------------------------------------------
static qcurses_alloc_t const sDefaultAllocator = {
  &qcurses_host_allocate,
  &qcurses_host_reallocate,
  &qcurses_host_free
};

////////////////////////////////////////////////////////////////////////////////
// QCurses Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void * qcurses_allocate_unsafe (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  return pAllocator->pfnAllocate(pAllocator, n, align);
}

//------------------------------------------------------------------------------
void * qcurses_reallocate_unsafe (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  return pAllocator->pfnReallocate(pAllocator, ptr, n);
}

//------------------------------------------------------------------------------
void qcurses_free_unsafe (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
) {
  pAllocator->pfnFree(pAllocator, ptr);
}

//------------------------------------------------------------------------------
void * qcurses_allocate (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  if (!pAllocator)
    return qcurses_host_allocate(pAllocator, n, align);
  return pAllocator->pfnAllocate(pAllocator, n, align);
}

//------------------------------------------------------------------------------
void * qcurses_reallocate (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  if (!pAllocator)
    return qcurses_host_reallocate(pAllocator, ptr, n);
  return pAllocator->pfnReallocate(pAllocator, ptr, n);
}

//------------------------------------------------------------------------------
void qcurses_free (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
) {
  if (!pAllocator)
    qcurses_host_free(pAllocator, ptr);
  else
    pAllocator->pfnFree(pAllocator, ptr);
}

//------------------------------------------------------------------------------
void qcurses_host_allocator_init (
  qcurses_alloc_t *                     pAllocator
) {
  pAllocator->pfnAllocate = &qcurses_host_allocate;
  pAllocator->pfnReallocate = &qcurses_host_reallocate;
  pAllocator->pfnFree = &qcurses_host_free;
}

//------------------------------------------------------------------------------
qcurses_alloc_t const * qcurses_default_allocator () {
  return &sDefaultAllocator;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
