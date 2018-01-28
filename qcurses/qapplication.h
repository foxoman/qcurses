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
#ifndef   QAPPLICATION_H
#define   QAPPLICATION_H

#include "qcurses.h"
#include "qwidget.h"
#include "qmenu_bar.h"
#include "qstatus_bar.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Application Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct qapplication_info_t {
  qalloc_t const *                      pAllocator;
  char const *                          pApplicationName;
  char const *                          pDescription;
  char const *                          pVersion;
  char const *                          pCopyright;
};

////////////////////////////////////////////////////////////////////////////////
// Application Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QWIDGET_BEGIN(qapplication_t)
  QWIDGET_SIGNALS_BEGIN
    QSIGNAL(quit, int code);
    QSIGNAL(resize, qbounds_t const * bounds);
    QSIGNAL(on_key, qkey_t code, int value);
    QSIGNAL(on_mouse, qcoord_t const * coord, qmouse_t state);
  QWIDGET_SIGNALS_END
QWIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Application Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_application (
  qapplication_info_t const *           pCreateInfo,
  qapplication_t **                     pApplication
);

//------------------------------------------------------------------------------
void QCURSESCALL qdestroy_application (
  qapplication_t *                      pApplication
);

//------------------------------------------------------------------------------
qwidget_t * QCURSESCALL qapplication_get_main_widget (
  qapplication_t *                      pApplication
);

//------------------------------------------------------------------------------
int QCURSESCALL __qapplication_set_main_widget (
  qapplication_t *                      pApplication,
  qwidget_t *                           pWidget
);

//------------------------------------------------------------------------------
#define qapplication_set_main_widget(pApplication, pWidget)                     \
  __qapplication_set_main_widget(                                               \
    pApplication,                                                               \
    ((qwidget_t *)pWidget)                                                      \
  )

//------------------------------------------------------------------------------
qmenu_bar_t * QCURSESCALL qapplication_get_menu_bar (
  qapplication_t *                      pApplication
);

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_set_menu_bar (
  qapplication_t *                      pApplication,
  qmenu_bar_t *                         pMenuBar
);

//------------------------------------------------------------------------------
qstatus_bar_t * QCURSESCALL qapplication_get_status_bar (
  qapplication_t *                      pApplication
);

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_set_status_bar (
  qapplication_t *                      pApplication,
  qstatus_bar_t *                       pStatusBar
);

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_run (
  qapplication_t *                      pApplication
);

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_quit (
  qapplication_t *                      pApplication,
  int                                   code
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QAPPLICATION_H
