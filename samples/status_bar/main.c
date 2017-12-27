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
 * Sample Slate application with minimal UI (status bar and central label).
 ******************************************************************************/

#include <slate/slate.h>
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

static int update_application_quit (slate_application_t * app, slate_keycode_t code, int value) {
  (void)app;
  (void)value;
  return (code == SLATE_KEYCODE_Q) ? SLATE_TERMINATE : 0;
}

static int label_set_text_key (slate_label_t * label, slate_keycode_t code, int value) {
  int bytesWritten;
  bytesWritten = sprintf(s_buffer, "Keyboard: (value: %d, code: 0x%x)", value, code);
  if (bytesWritten < 0) {
    return -1;
  }
  SLATE_CHECK(slate_label_set_text_n(label, s_buffer, (size_t)bytesWritten));
  return 0;
}

static int label_set_text_resize (slate_label_t * label, uint32_t screenRows, uint32_t screenColumns) {
  int bytesWritten;
  bytesWritten = sprintf(s_buffer, "[%ux%u]", screenColumns, screenRows);
  if (bytesWritten < 0) {
    return -1;
  }
  SLATE_CHECK(slate_label_set_text_n(label, s_buffer, (size_t)bytesWritten));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

int main_prepare_status_bar (slate_alloc_t const * pAllocator, slate_application_t * app) {
  slate_label_t * label;
  slate_status_bar_t * statusBar;

  // Create a status bar for our application.
  SLATE_CHECK(slate_create_status_bar(pAllocator, &statusBar));

  // Create a label which will sit in the status bar.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_LEFT_BIT));
  SLATE_CHECK(slate_widget_connect(app, SLATE_APPLICATION_ONKEY, label, &label_set_text_key));
  SLATE_CHECK(slate_widget_insert(statusBar, label));

  // Add a second label, they should both now take up half of the screen.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_RIGHT_BIT));
  SLATE_CHECK(slate_widget_connect(app, SLATE_APPLICATION_ONRESIZE, label, &label_set_text_resize));
  SLATE_CHECK(slate_widget_insert(statusBar, label));

  // Add the label to the status bar, then add it to the app.
  SLATE_CHECK(slate_application_set_status_bar(app, statusBar));

  return 0;
}

int main_prepare_central_widget (slate_alloc_t const * pAllocator, slate_application_t * app) {
  slate_label_t * label;

  // Create a label for information.
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_CENTER_BIT | SLATE_ALIGN_MIDDLE_BIT));
  SLATE_CHECK(slate_label_set_text_k(label, "Press any key, or resize the window. Press Q to quit."));
  SLATE_CHECK(slate_widget_insert(app, label));

  return 0;
}

int main_prepare_application (slate_alloc_t const * pAllocator, slate_application_t * app) {
  SLATE_CHECK(slate_widget_connect(app, SLATE_APPLICATION_ONKEY, app, &update_application_quit));
  SLATE_CHECK(main_prepare_central_widget(pAllocator, app));
  SLATE_CHECK(main_prepare_status_bar(pAllocator, app));
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Main Entry-Point
////////////////////////////////////////////////////////////////////////////////

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
  SLATE_CHECK(slate_run_application(app));
  SLATE_CHECK(slate_destroy_application(app));

  return 0;
}
