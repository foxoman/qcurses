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

// TODO: Support wide-character output so that I can use UNICODE and extended characters. (req CMake 3.10+)
static char const s_brushweights[] = " .oO0";
#define CANVAS_MAX_VALUE (sizeof(s_brushweights) / sizeof(s_brushweights[0]) - 2)
#define CANVAS_MIN_VALUE 0
#define CANVAS_BRUSH_WEIGHT  1.00f
#define CANVAS_ERASE_WEIGHT -1.00f

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
  int                                   fullRefresh;
  float                                 brushValue;
  qcurses_coord_t                       currCoord;
  qcurses_coord_t                       prevCoord;
  float *                               pBuffer;
};

//------------------------------------------------------------------------------
QCURSES_RECALC(
  canvas_widget_recalculate,
  canvas_widget_t *                     pThis,
  qcurses_region_t const *              pRegion
) {
  float * newBuffer;
  size_t bufferSize;

  // Update the region information (no special region handling here).
  W(pThis)->contentBounds =  pRegion->bounds;
  W(pThis)->innerRegion   = *pRegion;
  W(pThis)->outerRegion   = *pRegion;

  // Grow the internal buffer to handle the new buffer size.
  bufferSize = sizeof(P(pThis)->pBuffer[0]) * pRegion->bounds.rows * pRegion->bounds.columns;
  newBuffer = qcurses_reallocate(
    W(pThis)->pAllocator,
    P(pThis)->pBuffer,
    bufferSize
  );
  if (!newBuffer) {
    return ENOMEM;
  }

  // Initialize the new buffer size, and schedule a screen refresh.
  // Note: Currently we don't preserve the buffer, so we can lose data here.
  qcurses_widget_mark_dirty(pThis);
  memset(newBuffer, 0, bufferSize);
  P(pThis)->pBuffer = newBuffer;

  return 0;
}

//------------------------------------------------------------------------------
QCURSES_PAINTER(
  canvas_widget_paint,
  canvas_widget_t *                     pThis,
  qcurses_painter_t *                   pPainter
) {
  int finalValue;
  size_t idxOffset;

  // If the dirty coordinate is the same as the previous coordinate
  // then there is nothing to update (we don't update the same cell twice).
  if (qcurses_coord_equal(&P(pThis)->prevCoord, &P(pThis)->currCoord)) {
    return 0;
  }

  // Calculate the index into the canvas buffer, and add the brush value.
  // This value should be clamped to the min/max canvas value as well.
  idxOffset  = W(pThis)->contentBounds.columns * P(pThis)->currCoord.row;
  idxOffset += P(pThis)->currCoord.column;
  P(pThis)->pBuffer[idxOffset] += P(pThis)->brushValue;
  if (P(pThis)->pBuffer[idxOffset] > CANVAS_MAX_VALUE) {
    P(pThis)->pBuffer[idxOffset] = CANVAS_MAX_VALUE;
  }
  if (P(pThis)->pBuffer[idxOffset] < CANVAS_MIN_VALUE) {
    P(pThis)->pBuffer[idxOffset] = CANVAS_MIN_VALUE;
  }

  // Update the character on-screen which has been updated by the logic.
  P(pThis)->prevCoord = P(pThis)->currCoord;
  finalValue = (int)(P(pThis)->pBuffer[idxOffset]);
  return qcurses_painter_paint(
    pPainter,
    &P(pThis)->currCoord,
    &s_brushweights[finalValue],
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
  canvas_widget_click,
  canvas_widget_t *                     pThis,
  qcurses_coord_t const *               pCoord,
  qcurses_mouse_t                       buttonState
) {

  // Brush:
  if (buttonState & QCURSES_MOUSE_BUTTON_LEFT_PRESSED_BIT) {
    P(pThis)->brushValue = CANVAS_BRUSH_WEIGHT;
    P(pThis)->currCoord = *pCoord;
    qcurses_widget_mark_dirty(pThis);
  }

  // Eraser:
  if (buttonState & QCURSES_MOUSE_BUTTON_RIGHT_PRESSED_BIT) {
    P(pThis)->brushValue = CANVAS_ERASE_WEIGHT;
    P(pThis)->currCoord = *pCoord;
    qcurses_widget_mark_dirty(pThis);
  }

  return 0;
}

//------------------------------------------------------------------------------
static void qcurses_destroy_canvas (
  canvas_widget_t *                     canvas
) {
  free(P(canvas)->pBuffer);
  qcurses_destroy_widget(canvas);
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
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(canvas_widget_t);
  widgetConfig.privateSize    = sizeof(QCURSES_PIMPL_STRUCT(canvas_widget_t));
  widgetConfig.pfnDestroy     = QCURSES_DESTROY_PTR(qcurses_destroy_canvas);
  widgetConfig.pfnRecalculate = QCURSES_RECALC_PTR(canvas_widget_recalculate);
  widgetConfig.pfnPaint       = QCURSES_PAINTER_PTR(canvas_widget_paint);

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &canvas
  );
  if (err) {
    return err;
  }

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
