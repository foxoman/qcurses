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
 * Slate Terminal UI Source
 ******************************************************************************/

#include "status_bar.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Public Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int slate_status_bar_recalculate (
  slate_status_bar_t *                  pStatusBar,
  slate_bounds_t const *                pBounds
) {
  (void)pStatusBar;
  (void)pBounds;
  return ENOTSUP;
}

//------------------------------------------------------------------------------
static int slate_status_bar_paint (
  slate_status_bar_t *                  pStatusBar,
  slate_region_t const *                pRegion
) {
  (void)pStatusBar;
  (void)pRegion;
  return ENOTSUP;
}

//------------------------------------------------------------------------------
int slate_create_status_bar (
  slate_alloc_t const *                 pAllocator,
  slate_status_bar_t **                 pStatusBar
) {
  (void)pAllocator;
  (void)pStatusBar;
  return ENOTSUP;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
