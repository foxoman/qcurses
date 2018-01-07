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
 * Sample Slate application with minimal UI (status bar and main label).
 ******************************************************************************/

#include <slate/slate.h>
#include <slate/application.h>
#include <slate/label.h>
#include <slate/status_bar.h>
#include <string.h>
#include <stdio.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "status_bar"
#define APPLICATION_COPYRIGHT   "Trent Reed 2017"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A simple application with minimal UI."

#define SLATE_CHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Application Globals
////////////////////////////////////////////////////////////////////////////////

static char s_buffer[1024];

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
SLATE_SLOT(
  application_quit,
  slate_application_t *                 pThis,
) {
  slate_signal_key_t const *            pParams

  // If the user presses "Q", we should prepare to quit.
  if (pParams->code == SLATE_KEYCODE_Q) {
    return slate_application_quit(pThis);
  }

  return 0;
}

//------------------------------------------------------------------------------
SLATE_SLOT(
  label_show_key,
  slate_label_t *                       pThis,
  slate_signal_key_t const *            pParams
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
  SLATE_CHECK(slate_label_set_text_n(pThis, s_buffer, (size_t)bytesWritten));
  return 0;
}

//------------------------------------------------------------------------------
SLATE_SLOT(
  label_show_size,
  slate_label_t *                       pThis,
  slate_signal_resize_t const *         pParams
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
  SLATE_CHECK(slate_label_set_text_n(pThis, s_buffer, (size_t)bytesWritten));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_status_bar (
  slate_alloc_t const *                 pAllocator,
  slate_application_t *                 pApplication
) {
  slate_label_t * label;
  slate_status_bar_t * statusBar;

  // Create a status bar for our application.
  SLATE_CHECK(slate_create_status_bar(pAllocator, &statusBar));

  // Create a label which will sit in the status bar.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_LEFT_BIT));
  SLATE_CHECK(slate_widget_connect(pApplication, onKey, label, label_show_key));
  SLATE_CHECK(slate_status_bar_insert(statusBar, label));

  // Add a second label, they should both now take up half of the screen.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_RIGHT_BIT));
  SLATE_CHECK(slate_widget_connect(pApplication, onResize, label, label_show_size));
  SLATE_CHECK(slate_status_bar_insert(statusBar, label));

  // Add the label to the status bar, then add it to the app.
  SLATE_CHECK(slate_application_set_status_bar(pApplication, statusBar));

  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  slate_alloc_t const *                 pAllocator,
  slate_application_t *                 pApplication
) {
  slate_label_t * label;

  // Create a label for information.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_CENTER_BIT | SLATE_ALIGN_MIDDLE_BIT));
  SLATE_CHECK(slate_label_set_text_k(label, "Press any key, or resize the window. Press Q to quit."));
  SLATE_CHECK(slate_application_set_main_widget(pApplication, label));

  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_application (
  slate_alloc_t const *                 pAllocator,
  slate_application_t *                 pApplication
) {
  SLATE_CHECK(slate_widget_connect(pApplication, onKey, pApplication, application_quit));
  SLATE_CHECK(main_prepare_main_widget(pAllocator, pApplication));
  SLATE_CHECK(main_prepare_status_bar(pAllocator, pApplication));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Main Entry-Point
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int main (int argc, char const * argv[]) {
  slate_application_t* app;

  // Prepare the application for initialization.
  slate_application_info_t appInfo;
  memset(&appInfo, 0, sizeof(appInfo));
  appInfo.pAllocator        = NULL;
  appInfo.pApplicationName  = APPLICATION_NAME;
  appInfo.pCopyright        = APPLICATION_COPYRIGHT;
  appInfo.pVersion          = APPLICATION_VERSION;
  appInfo.pDescription      = APPLICATION_DESCRIPTION;

  // Run the application by creating, preparing, running, and destroying.
  SLATE_CHECK(slate_create_application(&appInfo, &app));
  SLATE_CHECK(main_prepare_application(appInfo.pAllocator, app));
  SLATE_CHECK(slate_application_run(app));
  slate_destroy_application(app);

  return 0;
}
