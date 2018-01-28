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
#include <qcurses/qlayout.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "vertical-layout"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A minimal application using labels and vertical layouts."

#define QCHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QSLOT(
  application_quit,
  qapplication_t *                      pThis,
  qkey_t                                code,
  int                                   value
) {
  (void)value;

  // If the user presses "Q", we should prepare to quit.
  if (code == QKEY_Q) {
    return qapplication_quit(pThis, 0);
  }

  return 0;
}

//------------------------------------------------------------------------------
QSLOT(
  layout_reorganize,
  qlayout_t *                           pThis,
  qkey_t                                code,
  int                                   value
) {
  (void)value;

  // If the user presses "Q", we should prepare to quit.
  if (code == QKEY_1) {
    return qlayout_set_format(pThis, QLAYOUT_VERTICAL);
  }
  if (code == QKEY_2) {
    return qlayout_set_format(pThis, QLAYOUT_VERTICAL_REVERSE);
  }
  if (code == QKEY_3) {
    return qlayout_set_format(pThis, QLAYOUT_HORIZONTAL);
  }
  if (code == QKEY_4) {
    return qlayout_set_format(pThis, QLAYOUT_HORIZONTAL_REVERSE);
  }

  return 0;
}

//------------------------------------------------------------------------------
QSLOT(
  layout_changed,
  qlabel_t *                            pThis,
  qlayout_format_t                      format
) {
  switch (format) {
    case QLAYOUT_VERTICAL:
    case QLAYOUT_VERTICAL_REVERSE:
      return qlabel_set_text_k(pThis, "Vertical");
    case QLAYOUT_HORIZONTAL:
    case QLAYOUT_HORIZONTAL_REVERSE:
      return qlabel_set_text_k(pThis, "Horizontal");
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define main_create_attach_label_k(pAllocator, layout, str) do {                \
  QCHECK(qcreate_label(pAllocator, &label));                                    \
  QCHECK(qlabel_set_text_k(label, str));                                        \
  QCHECK(qlayout_add_widget(layout, label, 0));                                 \
} while (0)

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  qlabel_t * label;
  qlayout_t * layout;
  QCHECK(qcreate_layout(pAllocator, QLAYOUT_VERTICAL, &layout));
  QCHECK(qwidget_connect(pApplication, on_key, layout, layout_reorganize));
  main_create_attach_label_k(pAllocator, layout, "This");
  main_create_attach_label_k(pAllocator, layout, "ia a");
  main_create_attach_label_k(pAllocator, layout, "Vertical");
  QCHECK(qwidget_connect(layout, set_format, label, layout_changed));
  main_create_attach_label_k(pAllocator, layout, "Layout");
  QCHECK(qapplication_set_main_widget(pApplication, layout));
  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_application (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  (void)pAllocator;
  QCHECK(qwidget_connect(pApplication, on_key, pApplication, application_quit));
  QCHECK(main_prepare_main_widget(pAllocator, pApplication));
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
