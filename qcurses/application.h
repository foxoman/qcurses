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
#ifndef   QCURSES_APPLICATION_H
#define   QCURSES_APPLICATION_H

#include "qcurses.h"
#include "widget.h"
#include "menu_bar.h"
#include "status_bar.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Application Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct qcurses_application_info_t {
  qcurses_alloc_t const *               pAllocator;
  char const *                          pApplicationName;
  char const *                          pDescription;
  char const *                          pVersion;
  char const *                          pCopyright;
};

////////////////////////////////////////////////////////////////////////////////
// Application Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_WIDGET_BEGIN(qcurses_application_t, qcurses_widget_t)
  QCURSES_WIDGET_SIGNALS(
    QCURSES_SIGNAL(onKey,               qcurses_keycode_t code, int value);
    QCURSES_SIGNAL(onResize,            qcurses_bounds_t const *);
    QCURSES_SIGNAL(onMouse,             qcurses_coord_t const *, qcurses_mouse_t buttons);
  )
QCURSES_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Application Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int qcurses_create_application (
  qcurses_application_info_t const *    pCreateInfo,
  qcurses_application_t **              pApplication
);

//------------------------------------------------------------------------------
void qcurses_destroy_application (
  qcurses_application_t *               pApplication
);

//------------------------------------------------------------------------------
qcurses_widget_t * qcurses_application_get_main_widget (
  qcurses_application_t *               pApplication
);

//------------------------------------------------------------------------------
int __qcurses_application_set_main_widget (
  qcurses_application_t *               pApplication,
  qcurses_widget_t *                    pWidget
);

//------------------------------------------------------------------------------
#define qcurses_application_set_main_widget(pApplication, pWidget)              \
  __qcurses_application_set_main_widget(                                        \
    pApplication,                                                               \
    (qcurses_widget_t *)pWidget                                                 \
  )

//------------------------------------------------------------------------------
qcurses_menu_bar_t * qcurses_application_get_menu_bar (
  qcurses_application_t *               pApplication
);

//------------------------------------------------------------------------------
int qcurses_application_set_menu_bar (
  qcurses_application_t *               pApplication,
  qcurses_menu_bar_t *                  pMenuBar
);

//------------------------------------------------------------------------------
qcurses_status_bar_t * qcurses_application_get_status_bar (
  qcurses_application_t *               pApplication
);

//------------------------------------------------------------------------------
int qcurses_application_set_status_bar (
  qcurses_application_t *               pApplication,
  qcurses_status_bar_t *                pStatusBar
);

//------------------------------------------------------------------------------
int qcurses_application_run (
  qcurses_application_t *               pApplication
);

//------------------------------------------------------------------------------
int qcurses_application_quit (
  qcurses_application_t *               pApplication
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_APPLICATION_H
