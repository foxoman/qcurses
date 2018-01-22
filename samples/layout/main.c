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
#include <qcurses/label.h>
#include <qcurses/layout.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "vertical-layout"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A minimal application using labels and vertical layouts."

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

//------------------------------------------------------------------------------
QCURSES_SLOT(
  layout_reorganize,
  qcurses_layout_t *                    pThis,
  qcurses_keycode_t                     code,
  int                                   value
) {
  (void)value;

  // If the user presses "Q", we should prepare to quit.
  if (code == QCURSES_KEYCODE_1) {
    return qcurses_layout_set_format(pThis, QCURSES_LAYOUT_VERTICAL);
  }
  if (code == QCURSES_KEYCODE_2) {
    return qcurses_layout_set_format(pThis, QCURSES_LAYOUT_VERTICAL_REVERSE);
  }
  if (code == QCURSES_KEYCODE_3) {
    return qcurses_layout_set_format(pThis, QCURSES_LAYOUT_HORIZONTAL);
  }
  if (code == QCURSES_KEYCODE_4) {
    return qcurses_layout_set_format(pThis, QCURSES_LAYOUT_HORIZONTAL_REVERSE);
  }

  return 0;
}

//------------------------------------------------------------------------------
QCURSES_SLOT(
  layout_changed,
  qcurses_label_t *                     pThis,
  qcurses_layout_format_t               format
) {
  switch (format) {
    case QCURSES_LAYOUT_VERTICAL:
    case QCURSES_LAYOUT_VERTICAL_REVERSE:
      return qcurses_label_set_text_k(pThis, "Vertical");
    case QCURSES_LAYOUT_HORIZONTAL:
    case QCURSES_LAYOUT_HORIZONTAL_REVERSE:
      return qcurses_label_set_text_k(pThis, "Horizontal");
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define main_create_attach_label_k(pAllocator, layout, str) do {                \
  QCURSES_CHECK(qcurses_create_label(pAllocator, &label));                      \
  QCURSES_CHECK(qcurses_label_set_text_k(label, str));                          \
  QCURSES_CHECK(qcurses_layout_add_widget(layout, label, 0));                   \
} while (0)

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  qcurses_alloc_t const *               pAllocator,
  qcurses_application_t *               pApplication
) {
  qcurses_label_t * label;
  qcurses_layout_t * layout;
  QCURSES_CHECK(qcurses_create_layout(pAllocator, QCURSES_LAYOUT_VERTICAL, &layout));
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onKey, layout, layout_reorganize));
  main_create_attach_label_k(pAllocator, layout, "This");
  main_create_attach_label_k(pAllocator, layout, "ia a");
  main_create_attach_label_k(pAllocator, layout, "Vertical");
  QCURSES_CHECK(qcurses_widget_connect(layout, onFormat, label, layout_changed));
  main_create_attach_label_k(pAllocator, layout, "Layout");
  QCURSES_CHECK(qcurses_application_set_main_widget(pApplication, layout));
  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_application (
  qcurses_alloc_t const *               pAllocator,
  qcurses_application_t *               pApplication
) {
  (void)pAllocator;
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onKey, pApplication, application_quit));
  QCURSES_CHECK(main_prepare_main_widget(pAllocator, pApplication));
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
