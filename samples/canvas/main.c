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
 * QCurses application with custom widget (a simple terminal paint app).
 ******************************************************************************/

#include <qcurses/painter.h>
#include <qcurses/qcurses.h>
#include <qcurses/application.h>
#include <qcurses/label.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Application Definitions
////////////////////////////////////////////////////////////////////////////////

#define APPLICATION_NAME        "canvas"
#define APPLICATION_COPYRIGHT   "Trent Reed 2018"
#define APPLICATION_VERSION     "v0.1"
#define APPLICATION_DESCRIPTION "A simple terminal-based paint application."

#define CANVAS_DEFAULT_BRUSH '.'

#define QCURSES_CHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Canvas Widget
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_WIDGET_BEGIN(canvas_widget_t, qcurses_widget_t)
  QCURSES_WIDGET_SIGNALS(
    QCURSES_SIGNAL(setBrush, char brush);
  )
QCURSES_WIDGET_END

//------------------------------------------------------------------------------
QCURSES_PIMPL_STRUCT(canvas_widget_t) {
  qcurses_coord_t                       dirtyCoord;
  char                                  brush;
};

//------------------------------------------------------------------------------
QCURSES_RECALC(
  canvas_widget_recalculate,
  canvas_widget_t *                     pThis,
  qcurses_region_t const *              pRegion
) {

  // Update the region information (no special region handling here).
  // TODO: This is confusing to configure, there has to be an easier way than recalc.
  W(pThis)->contentBounds = pRegion->bounds;
  W(pThis)->innerRegion = *pRegion;
  W(pThis)->outerRegion = *pRegion;

  return 0;
}

//------------------------------------------------------------------------------
QCURSES_PAINTER(
  canvas_widget_paint,
  canvas_widget_t *                     pThis,
  qcurses_painter_t *                   pPainter
) {

  // If the dirty coordinate is outside of the valid bounds,
  // then we aren't actually processing a valid dirty bit.
  if (!qcurses_bounds_contains(&W(pThis)->contentBounds, &P(pThis)->dirtyCoord)) {
    return 0;
  }

  // Otherwise, we should paint the current brush value to the screen.
  // TODO: In the future, we should have a local temporary buffer so we don't lose data on resize.
  //       This will also allow us to "save" the buffer so that we can save our work.
  return qcurses_painter_paint(
    pPainter,
    &P(pThis)->dirtyCoord,
    &P(pThis)->brush,
    1
  );
}

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
// Canvas Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_SLOT(
  canvas_widget_brush,
  canvas_widget_t *                     pThis,
  qcurses_keycode_t                     code,
  int                                   value
) {
  P(pThis)->brush = (char)value;
  qcurses_widget_emit(pThis, setBrush, (char)value);
  return 0;
}

//------------------------------------------------------------------------------
QCURSES_SLOT(
  canvas_widget_click,
  canvas_widget_t *                     pThis,
  qcurses_coord_t const *               pCoord,
  qcurses_mouse_t                       buttonState
) {
  if (buttonState & QCURSES_MOUSE_BUTTON1_PRESSED_BIT) {
    P(pThis)->dirtyCoord = *pCoord;
    qcurses_widget_mark_dirty(pThis);
  }
  return 0;
}

//------------------------------------------------------------------------------
static int create_canvas_widget (
  qcurses_alloc_t const *               pAllocator,
  canvas_widget_t **                    pCanvasWidget
) {
  int err;
  qcurses_widget_config_t widgetConfig;
  canvas_widget_t * canvas;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator = pAllocator;
  widgetConfig.publicSize = sizeof(canvas_widget_t);
  widgetConfig.privateSize = sizeof(QCURSES_PIMPL_STRUCT(canvas_widget_t));
  widgetConfig.pfnDestroy = (qcurses_widget_destroy_pfn)&qcurses_destroy_label;
  widgetConfig.pfnRecalculate = QCURSES_RECALC_PTR(canvas_widget_recalculate);
  widgetConfig.pfnPaint = QCURSES_PAINTER_PTR(canvas_widget_paint);

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &canvas
  );
  if (err) {
    return err;
  }

  // Configure the canvas defaults.
  // Note: We chose an invalid coordinate for the first dirtyCoord to specify clean.
  //       This is because we start out in a dirty state, but don't want to touch the canvas.
  P(canvas)->dirtyCoord = qcurses_coord(QCURSES_INFINITE, QCURSES_INFINITE);
  P(canvas)->brush = CANVAS_DEFAULT_BRUSH;

  // Return the application to the caller.
  *pCanvasWidget = canvas;
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
  canvas_widget_t * canvas;
  QCURSES_CHECK(create_canvas_widget(pAllocator, &canvas));
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onMouse, canvas, canvas_widget_click));
  QCURSES_CHECK(qcurses_widget_connect(pApplication, onKey, canvas, canvas_widget_brush));
  QCURSES_CHECK(qcurses_application_set_main_widget(pApplication, canvas));
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
