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
// QCurses Functions
////////////////////////////////////////////////////////////////////////////////
// TODO: For now, have an allocator abstraction layer, but don't implement the allocators.
//       Gotta figure out how to better utilize a pass-through for LT3 first.

void * qcurses_allocate (
  qcurses_alloc_t const *               pAllocator,
  size_t                                n,
  size_t                                align
) {
  return aligned_alloc(align, n);
}

void * qcurses_reallocate (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr,
  size_t                                n
) {
  return realloc(ptr, n);
}

void qcurses_free (
  qcurses_alloc_t const *               pAllocator,
  void *                                ptr
) {
  free(ptr);
}

#ifdef    __cplusplus
}
#endif // __cplusplus
