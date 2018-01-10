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
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "label"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A simple application using a single label widget."

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
  label_alignment,
  qcurses_label_t *                     pThis,
  qcurses_keycode_t                     code,
  int                                   value
) {
  qcurses_align_t origAlign;
  qcurses_align_t newAlign;
  (void)value;

  // Grab the alignment in case we use it.
  origAlign = qcurses_label_get_align(pThis);
  newAlign = origAlign;

  // Alter the alignment of the label via WASD.
  switch (code) {

    case QCURSES_KEYCODE_W: {
      newAlign &= ~QCURSES_ALIGN_VERTICAL_MASK;
      switch (origAlign & QCURSES_ALIGN_VERTICAL_MASK) {
        case QCURSES_ALIGN_TOP_BIT:
        case QCURSES_ALIGN_MIDDLE_BIT:
          newAlign |= QCURSES_ALIGN_TOP_BIT;
          break;
        case QCURSES_ALIGN_BOTTOM_BIT:
          newAlign |= QCURSES_ALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case QCURSES_KEYCODE_S: {
      newAlign &= ~QCURSES_ALIGN_VERTICAL_MASK;
      switch (origAlign & QCURSES_ALIGN_VERTICAL_MASK) {
        case QCURSES_ALIGN_BOTTOM_BIT:
        case QCURSES_ALIGN_MIDDLE_BIT:
          newAlign |= QCURSES_ALIGN_BOTTOM_BIT;
          break;
        case QCURSES_ALIGN_TOP_BIT:
          newAlign |= QCURSES_ALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case QCURSES_KEYCODE_A: {
      newAlign &= ~QCURSES_ALIGN_HORIZONTAL_MASK;
      switch (origAlign & QCURSES_ALIGN_HORIZONTAL_MASK) {
        case QCURSES_ALIGN_LEFT_BIT:
        case QCURSES_ALIGN_CENTER_BIT:
          newAlign |= QCURSES_ALIGN_LEFT_BIT;
          break;
        case QCURSES_ALIGN_RIGHT_BIT:
          newAlign |= QCURSES_ALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    case QCURSES_KEYCODE_D: {
      newAlign &= ~QCURSES_ALIGN_HORIZONTAL_MASK;
      switch (origAlign & QCURSES_ALIGN_HORIZONTAL_MASK) {
        case QCURSES_ALIGN_RIGHT_BIT:
        case QCURSES_ALIGN_CENTER_BIT:
          newAlign |= QCURSES_ALIGN_RIGHT_BIT;
          break;
        case QCURSES_ALIGN_LEFT_BIT:
          newAlign |= QCURSES_ALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    default:
      break;
  }

  // Mark the label as dirty.
  qcurses_widget_mark_dirty(pThis);
  if (newAlign != origAlign) {
    return qcurses_label_set_align(pThis, newAlign);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  qcurses_alloc_t const *               pAllocator,
  qcurses_application_t *               pApplication
) {
  qcurses_label_t * label;
  QCURSES_CHECK(qcurses_create_label(pAllocator, &label));
  QCURSES_CHECK(qcurses_label_set_align(label, QCURSES_ALIGN_CENTER_BIT | QCURSES_ALIGN_MIDDLE_BIT));
  QCURSES_CHECK(qcurses_label_set_text_k(label, "Use W, A, S, D to position the text.\nPress Q to quit."));
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onKey, label, label_alignment));
  QCURSES_CHECK(qcurses_application_set_main_widget(pApplication, label));
  return 0;
}

//------------------------------------------------------------------------------
static int main_prepare_application (
  qcurses_alloc_t const *               pAllocator,
  qcurses_application_t *               pApplication
) {
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
