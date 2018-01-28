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
#include <qcurses/qapplication.h>
#include <qcurses/qlabel.h>
#include <qcurses/qstatus_bar.h>
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "status_bar"
#define APPLICATION_COPYRIGHT   "Trent Reed 2017"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A simple application with minimal UI."

#define QCHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Application Globals
////////////////////////////////////////////////////////////////////////////////

static char s_buffer[1024];

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QSLOT(
  application_quit,
  qapplication_t *                      pThis,
  qsignal_key_t const *                 pParams
) {

  // If the user presses "Q", we should prepare to quit.
  if (pParams->code == QKEY_Q) {
    return qapplication_quit(pThis, 0);
  }

  return 0;
}

//------------------------------------------------------------------------------
QSLOT(
  label_show_key,
  qlabel_t *                            pThis,
  qsignal_key_t const *                 pParams
) {
  int bytesWritten;

  // Write into a common temporary buffer.
  // This is okay because signals are not async.
  bytesWritten = sprintf(
    s_buffer,
    "Keyboard: (value: %d, code: 0x%x)",
    pParams->value,
    pParams->code
  );
  if (bytesWritten < 0) {
    return -1;
  }

  // Present the resulting text in the label.
  QCHECK(qlabel_set_text_n(pThis, s_buffer, (size_t)bytesWritten));
  return 0;
}

//------------------------------------------------------------------------------
QSLOT(
  label_show_size,
  qlabel_t *                            pThis,
  qsignal_resize_t const *              pParams
) {
  int bytesWritten;

  // Write into a common temporary buffer.
  // This is okay because signals are not async.
  bytesWritten = sprintf(
    s_buffer,
    "[%ux%u]",
    (unsigned)pParams->columns,
    (unsigned)pParams->rows
  );
  if (bytesWritten < 0) {
    return -1;
  }

  // Present the resulting text in the label.
  QCHECK(qlabel_set_text_n(pThis, s_buffer, (size_t)bytesWritten));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_status_bar (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  qlabel_t * label;
  qstatus_bar_t * statusBar;

  // Create a status bar for our application.
  QCHECK(qcreate_status_bar(pAllocator, &statusBar));

  // Create a label which will sit in the status bar.
  QCHECK(qcreate_label(pAllocator, &label));
  QCHECK(qlabel_set_align(label, QALIGN_LEFT_BIT));
  QCHECK(qwidget_connect(pApplication, on_key, label, label_show_key));
  QCHECK(qstatus_bar_insert(statusBar, label));

  // Add a second label, they should both now take up half of the screen.
  QCHECK(qcreate_label(pAllocator, &label));
  QCHECK(qlabel_set_align(label, QALIGN_RIGHT_BIT));
  QCHECK(qwidget_connect(pApplication, resize, label, label_show_size));
  QCHECK(qstatus_bar_insert(statusBar, label));

  // Add the label to the status bar, then add it to the app.
  QCHECK(qapplication_set_status_bar(pApplication, statusBar));

  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  qlabel_t * label;

  // Create a label for information.
  QCHECK(qcreate_label(pAllocator, &label));
  QCHECK(qlabel_set_align(label, QALIGN_CENTER_BIT | QALIGN_MIDDLE_BIT));
  QCHECK(qlabel_set_text_k(label, "Press any key, or resize the window. Press Q to quit."));
  QCHECK(qapplication_set_main_widget(pApplication, label));

  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_application (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  QCHECK(qwidget_connect(pApplication, on_key, pApplication, application_quit));
  QCHECK(main_prepare_main_widget(pAllocator, pApplication));
  QCHECK(main_prepare_status_bar(pAllocator, pApplication));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Main Entry-Point
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int main (int argc, char const * argv[]) {
  qapplication_t* app;

  // Prepare the application for initialization.
  qapplication_info_t appInfo;
  memset(&appInfo, 0, sizeof(appInfo));
  appInfo.pAllocator        = NULL;
  appInfo.pApplicationName  = APPLICATION_NAME;
  appInfo.pCopyright        = APPLICATION_COPYRIGHT;
  appInfo.pVersion          = APPLICATION_VERSION;
  appInfo.pDescription      = APPLICATION_DESCRIPTION;

  // Run the application by creating, preparing, running, and destroying.
  QCHECK(qcreate_application(&appInfo, &app));
  QCHECK(main_prepare_application(appInfo.pAllocator, app));
  QCHECK(qapplication_run(app));
  qdestroy_application(app);

  return 0;
}
