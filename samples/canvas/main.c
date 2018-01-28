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

#include <qcurses/qpainter.h>
#include <qcurses/qcurses.h>
#include <qcurses/qapplication.h>
#include <qcurses/qlabel.h>
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

#define QCHECK(s) do { int err = s; if (err) return err; } while (0)

////////////////////////////////////////////////////////////////////////////////
// Canvas Widget
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QWIDGET_BEGIN(canvas_widget_t)
  // Intentionally Empty
QWIDGET_END

//------------------------------------------------------------------------------
QPIMPL_STRUCT(canvas_widget_t) {
  int                                   fullRefresh;
  float                                 brushValue;
  qcoord_t                              currCoord;
  qcoord_t                              prevCoord;
  float *                               pBuffer;
};

//------------------------------------------------------------------------------
QRECALC(
  canvas_widget_recalculate,
  canvas_widget_t *                     pThis,
  qregion_t const *                     pRegion
) {
  float * newBuffer;
  size_t bufferSize;

  // Update the region information (no special region handling here).
  QW(pThis)->contentBounds =  pRegion->bounds;
  QW(pThis)->innerRegion   = *pRegion;
  QW(pThis)->outerRegion   = *pRegion;
  if (QP(pThis)->pBuffer && qregion_equal(pRegion, &QW(pThis)->outerRegion)) {
    return 0;
  }

  // Grow the internal buffer to handle the new buffer size.
  bufferSize = sizeof(QP(pThis)->pBuffer[0]) * pRegion->bounds.rows * pRegion->bounds.columns;
  newBuffer = qreallocate(
    QW(pThis)->pAllocator,
    QP(pThis)->pBuffer,
    bufferSize
  );
  if (!newBuffer) {
    return ENOMEM;
  }

  // Initialize the new buffer size, and schedule a screen refresh.
  // Note: Currently we don't preserve the buffer, so we can lose data here.
  qwidget_mark_dirty(pThis);
  memset(newBuffer, 0, bufferSize);
  QP(pThis)->pBuffer = newBuffer;

  return 0;
}

//------------------------------------------------------------------------------
QPAINTER(
  canvas_widget_paint,
  canvas_widget_t *                     pThis,
  qpainter_t *                          pPainter
) {
  int finalValue;
  size_t idxOffset;

  // If the dirty coordinate is the same as the previous coordinate
  // then there is nothing to update (we don't update the same cell twice).
  if (qcoord_equal(&QP(pThis)->prevCoord, &QP(pThis)->currCoord)) {
    return 0;
  }

  // Calculate the index into the canvas buffer, and add the brush value.
  // This value should be clamped to the min/max canvas value as well.
  idxOffset  = QW(pThis)->contentBounds.columns * QP(pThis)->currCoord.row;
  idxOffset += QP(pThis)->currCoord.column;
  QP(pThis)->pBuffer[idxOffset] += QP(pThis)->brushValue;
  if (QP(pThis)->pBuffer[idxOffset] > CANVAS_MAX_VALUE) {
    QP(pThis)->pBuffer[idxOffset] = CANVAS_MAX_VALUE;
  }
  if (QP(pThis)->pBuffer[idxOffset] < CANVAS_MIN_VALUE) {
    QP(pThis)->pBuffer[idxOffset] = CANVAS_MIN_VALUE;
  }

  // Update the character on-screen which has been updated by the logic.
  QP(pThis)->prevCoord = QP(pThis)->currCoord;
  finalValue = (int)(QP(pThis)->pBuffer[idxOffset]);
  return qpainter_paint(
    pPainter,
    &QP(pThis)->currCoord,
    &s_brushweights[finalValue],
    1
  );
}

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

////////////////////////////////////////////////////////////////////////////////
// Canvas Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QSLOT(
  canvas_widget_click,
  canvas_widget_t *                     pThis,
  qcoord_t const *                      pCoord,
  qmouse_t                              buttonState
) {

  // Brush:
  if (buttonState & QMOUSE_LEFT_PRESSED_BIT) {
    QP(pThis)->brushValue = CANVAS_BRUSH_WEIGHT;
    QP(pThis)->currCoord = *pCoord;
    qwidget_mark_dirty(pThis);
  }

  // Eraser:
  if (buttonState & QMOUSE_RIGHT_PRESSED_BIT) {
    QP(pThis)->brushValue = CANVAS_ERASE_WEIGHT;
    QP(pThis)->currCoord = *pCoord;
    qwidget_mark_dirty(pThis);
  }

  return 0;
}

//------------------------------------------------------------------------------
static void qdestroy_canvas (
  canvas_widget_t *                     canvas
) {
  free(QP(canvas)->pBuffer);
  qdestroy_widget(canvas);
}

//------------------------------------------------------------------------------
static int create_canvas_widget (
  qalloc_t const *                      pAllocator,
  canvas_widget_t **                    pCanvasWidget
) {
  int err;
  qwidget_config_t widgetConfig;
  canvas_widget_t * canvas;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(canvas_widget_t);
  widgetConfig.privateSize    = sizeof(QPIMPL_STRUCT(canvas_widget_t));
  widgetConfig.pfnDestroy     = QDESTROY_PTR(qdestroy_canvas);
  widgetConfig.pfnRecalculate = QRECALC_PTR(canvas_widget_recalculate);
  widgetConfig.pfnPaint       = QPAINTER_PTR(canvas_widget_paint);

  // Allocate the terminal UI application.
  err = qcreate_widget(
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
  qalloc_t const *                      pAllocator,
  qapplication_t *                      pApplication
) {
  canvas_widget_t * canvas;
  QCHECK(create_canvas_widget(pAllocator, &canvas));
  QCHECK(qwidget_connect(pApplication, on_mouse, canvas, canvas_widget_click));
  QCHECK(qapplication_set_main_widget(pApplication, canvas));
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
