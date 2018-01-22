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
#ifndef   QCURSES_LAYOUT_H
#define   QCURSES_LAYOUT_H

#include "qcurses.h"
#include "widget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Layout Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
enum qcurses_layout_format_t {
  QCURSES_LAYOUT_VERTICAL,
  QCURSES_LAYOUT_VERTICAL_REVERSE,
  QCURSES_LAYOUT_HORIZONTAL,
  QCURSES_LAYOUT_HORIZONTAL_REVERSE
};

//------------------------------------------------------------------------------
QCURSES_WIDGET_BEGIN(qcurses_layout_t, qcurses_widget_t)
  QCURSES_WIDGET_SIGNALS(
    QCURSES_SIGNAL(onFormat, qcurses_layout_format_t);
  )
QCURSES_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Layout Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int qcurses_create_layout (
  qcurses_alloc_t const *               pAllocator,
  qcurses_layout_format_t               format,
  qcurses_layout_t **                   pLayout
);

//------------------------------------------------------------------------------
void qcurses_destroy_layout (
  qcurses_layout_t *                    pLayout
);

//------------------------------------------------------------------------------
qcurses_layout_format_t qcurses_layout_get_format (
  qcurses_layout_t const *              pLayout
);

//------------------------------------------------------------------------------
int qcurses_layout_set_format (
  qcurses_layout_t const *              pLayout,
  qcurses_layout_format_t               format
);

//------------------------------------------------------------------------------
int __qcurses_layout_add_widget (
  qcurses_layout_t *                    pLayout,
  qcurses_widget_t *                    pWidget,
  int                                   stretch
);

//------------------------------------------------------------------------------
#define qcurses_layout_add_widget(pLayout, pWidget, stretch)                    \
  __qcurses_layout_add_widget(                                                  \
    pLayout,                                                                    \
    ((qcurses_widget_t *)pWidget),                                              \
    stretch                                                                     \
  )

//------------------------------------------------------------------------------
int __qcurses_layout_insert_widget (
  qcurses_layout_t *                    pLayout,
  qcurses_widget_t *                    pWidget,
  int                                   stretch,
  uint32_t                              idx
);

//------------------------------------------------------------------------------
#define qcurses_layout_insert_widget(pLayout, pWidget, stretch, idx)            \
  __qcurses_layout_add_widget(                                                  \
    pLayout,                                                                    \
    ((qcurses_widget_t *)pWidget),                                              \
    stretch                                                                     \
    idx                                                                         \
  )

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_LAYOUT_H
