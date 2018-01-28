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
#ifndef   QLAYOUT_H
#define   QLAYOUT_H

#include "qcurses.h"
#include "qwidget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Layout Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
enum qlayout_format_t {
  QLAYOUT_VERTICAL,
  QLAYOUT_VERTICAL_REVERSE,
  QLAYOUT_HORIZONTAL,
  QLAYOUT_HORIZONTAL_REVERSE
};

// TODO: A layout should not be a widget, you would never want to connect to it.
//------------------------------------------------------------------------------
QWIDGET_BEGIN(qlayout_t)
  QWIDGET_SIGNALS_BEGIN
    QSIGNAL(set_format, qlayout_format_t);
  QWIDGET_SIGNALS_END
QWIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Layout Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_layout (
  qalloc_t const *                      pAllocator,
  qlayout_format_t                      format,
  qlayout_t **                          pLayout
);

//------------------------------------------------------------------------------
void QCURSESCALL qdestroy_layout (
  qlayout_t *                           pLayout
);

//------------------------------------------------------------------------------
qlayout_format_t QCURSESCALL qlayout_get_format (
  qlayout_t const *                     pLayout
);

//------------------------------------------------------------------------------
int QCURSESCALL qlayout_set_format (
  qlayout_t const *                     pLayout,
  qlayout_format_t                      format
);

//------------------------------------------------------------------------------
int QCURSESCALL __qlayout_add_widget (
  qlayout_t *                           pLayout,
  qwidget_t *                           pWidget,
  int                                   stretch
);

//------------------------------------------------------------------------------
#define qlayout_add_widget(pLayout, pWidget, stretch)                           \
  __qlayout_add_widget(                                                         \
    pLayout,                                                                    \
    ((qwidget_t *)pWidget),                                                     \
    stretch                                                                     \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qlayout_insert_widget (
  qlayout_t *                           pLayout,
  qwidget_t *                           pWidget,
  int                                   stretch,
  uint32_t                              idx
);

//------------------------------------------------------------------------------
#define qlayout_insert_widget(pLayout, pWidget, stretch, idx)                   \
  __qlayout_add_widget(                                                         \
    pLayout,                                                                    \
    ((qwidget_t *)pWidget),                                                     \
    stretch                                                                     \
    idx                                                                         \
  )

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QLAYOUT_H
