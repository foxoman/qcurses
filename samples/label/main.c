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
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "label"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A simple application using a single label widget."

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
  label_alignment,
  qlabel_t *                            pThis,
  qkey_t                                code,
  int                                   value
) {
  qalign_t origAlign;
  qalign_t newAlign;
  (void)value;

  // Grab the alignment in case we use it.
  origAlign = qlabel_get_align(pThis);
  newAlign = origAlign;

  // Alter the alignment of the label via WASD.
  switch (code) {

    case QKEY_W: {
      newAlign &= ~QALIGN_VERTICAL_MASK;
      switch (origAlign & QALIGN_VERTICAL_MASK) {
        case QALIGN_TOP_BIT:
        case QALIGN_MIDDLE_BIT:
          newAlign |= QALIGN_TOP_BIT;
          break;
        case QALIGN_BOTTOM_BIT:
          newAlign |= QALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case QKEY_S: {
      newAlign &= ~QALIGN_VERTICAL_MASK;
      switch (origAlign & QALIGN_VERTICAL_MASK) {
        case QALIGN_BOTTOM_BIT:
        case QALIGN_MIDDLE_BIT:
          newAlign |= QALIGN_BOTTOM_BIT;
          break;
        case QALIGN_TOP_BIT:
          newAlign |= QALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case QKEY_A: {
      newAlign &= ~QALIGN_HORIZONTAL_MASK;
      switch (origAlign & QALIGN_HORIZONTAL_MASK) {
        case QALIGN_LEFT_BIT:
        case QALIGN_CENTER_BIT:
          newAlign |= QALIGN_LEFT_BIT;
          break;
        case QALIGN_RIGHT_BIT:
          newAlign |= QALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    case QKEY_D: {
      newAlign &= ~QALIGN_HORIZONTAL_MASK;
      switch (origAlign & QALIGN_HORIZONTAL_MASK) {
        case QALIGN_RIGHT_BIT:
        case QALIGN_CENTER_BIT:
          newAlign |= QALIGN_RIGHT_BIT;
          break;
        case QALIGN_LEFT_BIT:
          newAlign |= QALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    default:
      break;
  }

  // Mark the label as dirty.
  qwidget_mark_dirty(pThis);
  if (newAlign != origAlign) {
    return qlabel_set_align(pThis, newAlign);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  qlabel_t * label;
  QCHECK(qcreate_label(pAllocator, &label));
  QCHECK(qlabel_set_align(label, QALIGN_CENTER_BIT | QALIGN_MIDDLE_BIT));
  QCHECK(qlabel_set_text_k(label, "Use W, A, S, D to position the text.\nPress Q to quit."));
  QCHECK(qwidget_connect(pApplication, on_key, label, label_alignment));
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
