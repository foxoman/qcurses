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
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "label"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v1.0"
#define APPLICATION_DESCRIPTION "A simple application using a single label widget."

#define SLATE_CHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
SLATE_SLOT(
  application_quit,
  slate_application_t *                 pThis,
  slate_keycode_t                       code,
  int                                   value
) {
  (void)value;

  // If the user presses "Q", we should prepare to quit.
  if (code == SLATE_KEYCODE_Q) {
    return slate_application_quit(pThis);
  }

  return 0;
}

//------------------------------------------------------------------------------
SLATE_SLOT(
  label_alignment,
  slate_label_t *                       pThis,
  slate_keycode_t                       code,
  int                                   value
) {
  slate_align_t origAlign;
  slate_align_t newAlign;
  (void)value;

  // Grab the alignment in case we use it.
  origAlign = slate_label_get_align(pThis);
  newAlign = origAlign;

  // Alter the alignment of the label via WASD.
  switch (code) {

    case SLATE_KEYCODE_W: {
      newAlign &= ~SLATE_ALIGN_VERTICAL_MASK;
      switch (origAlign & SLATE_ALIGN_VERTICAL_MASK) {
        case SLATE_ALIGN_TOP_BIT:
        case SLATE_ALIGN_MIDDLE_BIT:
          newAlign |= SLATE_ALIGN_TOP_BIT;
          break;
        case SLATE_ALIGN_BOTTOM_BIT:
          newAlign |= SLATE_ALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case SLATE_KEYCODE_S: {
      newAlign &= ~SLATE_ALIGN_VERTICAL_MASK;
      switch (origAlign & SLATE_ALIGN_VERTICAL_MASK) {
        case SLATE_ALIGN_BOTTOM_BIT:
        case SLATE_ALIGN_MIDDLE_BIT:
          newAlign |= SLATE_ALIGN_BOTTOM_BIT;
          break;
        case SLATE_ALIGN_TOP_BIT:
          newAlign |= SLATE_ALIGN_MIDDLE_BIT;
          break;
      }
    }
    break;

    case SLATE_KEYCODE_A: {
      newAlign &= ~SLATE_ALIGN_HORIZONTAL_MASK;
      switch (origAlign & SLATE_ALIGN_HORIZONTAL_MASK) {
        case SLATE_ALIGN_LEFT_BIT:
        case SLATE_ALIGN_CENTER_BIT:
          newAlign |= SLATE_ALIGN_LEFT_BIT;
          break;
        case SLATE_ALIGN_RIGHT_BIT:
          newAlign |= SLATE_ALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    case SLATE_KEYCODE_D: {
      newAlign &= ~SLATE_ALIGN_HORIZONTAL_MASK;
      switch (origAlign & SLATE_ALIGN_HORIZONTAL_MASK) {
        case SLATE_ALIGN_RIGHT_BIT:
        case SLATE_ALIGN_CENTER_BIT:
          newAlign |= SLATE_ALIGN_RIGHT_BIT;
          break;
        case SLATE_ALIGN_LEFT_BIT:
          newAlign |= SLATE_ALIGN_CENTER_BIT;
          break;
      }
    }
    break;

    default:
      break;
  }

  // Mark the label as dirty.
  slate_widget_mark_dirty(pThis);
  if (newAlign != origAlign) {
    return slate_label_set_align(pThis, newAlign);
  }

  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Preperation
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int main_prepare_main_widget (
  slate_alloc_t const *                 pAllocator,
  slate_application_t *                 pApplication
) {
  slate_label_t * label;
  SLATE_CHECK(slate_create_label(pAllocator, &label));
  SLATE_CHECK(slate_label_set_align(label, SLATE_ALIGN_CENTER_BIT | SLATE_ALIGN_MIDDLE_BIT));
  SLATE_CHECK(slate_label_set_text_k(label, "Use W, A, S, D to position the text.\nPress Q to quit."));
  SLATE_CHECK(slate_widget_connect(pApplication, onKey, label, label_alignment));
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
