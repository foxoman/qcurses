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
#ifndef   SLATE_STATUS_BAR_H
#define   SLATE_STATUS_BAR_H

#include "slate.h"
#include "widget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
SLATE_WIDGET_BEGIN(slate_status_bar_t, slate_widget_parent_t)
  // Intentionally Empty
SLATE_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int slate_create_status_bar (
  slate_alloc_t const *                 pAllocator,
  slate_status_bar_t **                 pStatusBar
);

//------------------------------------------------------------------------------
void slate_destroy_status_bar (
  slate_status_bar_t *                  pStatusBar
);

//------------------------------------------------------------------------------
static int __slate_status_bar_insert (
  slate_status_bar_t *                  pStatusBar,
  slate_widget_t *                      pWidget
) {
  (void)pStatusBar;
  (void)pWidget;
  return ENOTSUP;
}

//------------------------------------------------------------------------------
#define slate_status_bar_insert(pStatusBar, pWidget)                            \
  __slate_status_bar_insert(                                                    \
    pStatusBar,                                                                 \
    (slate_widget_t *)(pWidget)                                                 \
  )

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_STATUS_BAR_H
