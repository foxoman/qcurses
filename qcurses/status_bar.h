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
#ifndef   QCURSES_STATUS_BAR_H
#define   QCURSES_STATUS_BAR_H

#include "qcurses.h"
#include "widget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Status Bar Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_WIDGET_BEGIN(qcurses_status_bar_t, qcurses_widget_parent_t)
  // Intentionally Empty
QCURSES_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Status Bar Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int qcurses_create_status_bar (
  qcurses_alloc_t const *               pAllocator,
  qcurses_status_bar_t **               pStatusBar
);

//------------------------------------------------------------------------------
void qcurses_destroy_status_bar (
  qcurses_status_bar_t *                pStatusBar
);

//------------------------------------------------------------------------------
static int __qcurses_status_bar_insert (
  qcurses_status_bar_t *                pStatusBar,
  qcurses_widget_t *                    pWidget
) {
  (void)pStatusBar;
  (void)pWidget;
  return ENOTSUP;
}

//------------------------------------------------------------------------------
#define qcurses_status_bar_insert(pStatusBar, pWidget)                          \
  __qcurses_status_bar_insert(                                                  \
    pStatusBar,                                                                 \
    (qcurses_widget_t *)(pWidget)                                               \
  )

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_STATUS_BAR_H
