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
 * QCurses application with minimal UI (status bar and main label).
 ******************************************************************************/

#include <qcurses/qcurses.h>
#include <qcurses/application.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "empty"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A minimal application using no widgets."

#define QCURSES_CHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_SLOT(
  application_quit,
  qcurses_application_t *               pThis,
  qcurses_keycode_t                     code,
  int                                   value
) {
  (void)value;

  // If the user presses "Q", we should prepare to quit.
  if (code == QCURSES_KEYCODE_Q) {
    return qcurses_application_quit(pThis);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_application (
  qcurses_alloc_t const *               pAllocator,
  qcurses_application_t *               pApplication
) {
  (void)pAllocator;
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onKey, pApplication, application_quit));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Main Entry-Point
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int main (int argc, char const * argv[]) {
  qcurses_application_t* app;

  // Prepare the application for initialization.
  qcurses_application_info_t appInfo;
  memset(&appInfo, 0, sizeof(appInfo));
  appInfo.pAllocator        = NULL;
  appInfo.pApplicationName  = APPLICATION_NAME;
  appInfo.pCopyright        = APPLICATION_COPYRIGHT;
  appInfo.pVersion          = APPLICATION_VERSION;
  appInfo.pDescription      = APPLICATION_DESCRIPTION;

  // Run the application by creating, preparing, running, and destroying.
  QCURSES_CHECK(qcurses_create_application(&appInfo, &app));
  QCURSES_CHECK(main_prepare_application(appInfo.pAllocator, app));
  QCURSES_CHECK(qcurses_application_run(app));
  qcurses_destroy_application(app);

  return 0;
}
