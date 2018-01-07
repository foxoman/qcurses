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
#ifndef   SLATE_APPLICATION_H
#define   SLATE_APPLICATION_H

#include "slate.h"
#include "widget.h"
#include "menu_bar.h"
#include "status_bar.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct slate_application_info_t {
  slate_alloc_t const *                 pAllocator;
  char const *                          pApplicationName;
  char const *                          pDescription;
  char const *                          pVersion;
  char const *                          pCopyright;
};

////////////////////////////////////////////////////////////////////////////////
// Definition
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
SLATE_WIDGET_BEGIN(slate_application_t, slate_widget_t)
  SLATE_WIDGET_SIGNALS(
    SLATE_SIGNAL(onKey,                   slate_keycode_t code, int value);
    SLATE_SIGNAL(onResize,                slate_bounds_t const *);
  )
SLATE_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int slate_create_application (
  slate_application_info_t const *      pCreateInfo,
  slate_application_t **                pApplication
);

//------------------------------------------------------------------------------
void slate_destroy_application (
  slate_application_t *                 pApplication
);

//------------------------------------------------------------------------------
slate_widget_t * slate_application_get_main_widget (
  slate_application_t *                 pApplication
);

//------------------------------------------------------------------------------
int __slate_application_set_main_widget (
  slate_application_t *                 pApplication,
  slate_widget_t *                      pWidget
);

//------------------------------------------------------------------------------
#define slate_application_set_main_widget(pApplication, pWidget)                \
  __slate_application_set_main_widget(                                          \
    pApplication,                                                               \
    (slate_widget_t *)pWidget                                                   \
  )

//------------------------------------------------------------------------------
slate_menu_bar_t * slate_application_get_menu_bar (
  slate_application_t *                 pApplication
);

//------------------------------------------------------------------------------
int slate_application_set_menu_bar (
  slate_application_t *                 pApplication,
  slate_menu_bar_t *                    pMenuBar
);

//------------------------------------------------------------------------------
slate_status_bar_t * slate_application_get_status_bar (
  slate_application_t *                 pApplication
);

//------------------------------------------------------------------------------
int slate_application_set_status_bar (
  slate_application_t *                 pApplication,
  slate_status_bar_t *                  pStatusBar
);

//------------------------------------------------------------------------------
int slate_application_run (
  slate_application_t *                 pApplication
);

//------------------------------------------------------------------------------
int slate_application_quit (
  slate_application_t *                 pApplication
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_APPLICATION_H
