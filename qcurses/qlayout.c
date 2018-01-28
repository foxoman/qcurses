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
 ******************************************************************************/

#include "qlayout.h"

// TODO: Support element stretching, margins, layout properties.
////////////////////////////////////////////////////////////////////////////////
// Layout Implementations
////////////////////////////////////////////////////////////////////////////////

struct qlayout_element_t;
typedef struct qlayout_element_t {
  struct qlayout_element_t *     pNext;
  struct qlayout_element_t *     pPrevious;
  qwidget_t *                    pWidget;
  int                                   stretch;
} qlayout_element_t;

//------------------------------------------------------------------------------
struct QPIMPL_NAME(qlayout_t) {
  qlayout_element_t *            pWidgetsFirst;
  qlayout_element_t *            pWidgetsLast;
  qlayout_format_t               layoutFormat;
  size_t                                widgetCount;
};

//------------------------------------------------------------------------------
static int __qlayout_reacalculate_vertical (
  qlayout_t *                    pLayout,
  qregion_t const *              pRegion
) {
  int err;
  qlayout_element_t * pElement;
  qregion_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.rows / QP(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.rows - (boundsIncrement * QP(pLayout)->widgetCount);
  subRegion = qregion(
    pRegion->coord.column,
    pRegion->coord.row,
    boundsIncrement,
    pRegion->bounds.columns
  );

  // Calculate the region information per-widget
  pElement = QP(pLayout)->pWidgetsFirst;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pNext) {
      subRegion.bounds.rows += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qwidget_recalculate(pElement->pWidget, &subRegion);
    if (err) {
      return err;
    }

    // Advance the sub-region to the next widget.
    subRegion.coord.row += boundsIncrement;
    pElement = pElement->pNext;

  }

  return 0;
}

//------------------------------------------------------------------------------
static int __qlayout_reacalculate_vertical_reverse (
  qlayout_t *                    pLayout,
  qregion_t const *              pRegion
) {
  int err;
  qlayout_element_t * pElement;
  qregion_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.rows / QP(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.rows - (boundsIncrement * QP(pLayout)->widgetCount);
  subRegion = qregion(
    pRegion->coord.column,
    pRegion->coord.row,
    boundsIncrement,
    pRegion->bounds.columns
  );

  // Calculate the region information per-widget
  pElement = QP(pLayout)->pWidgetsLast;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pPrevious) {
      subRegion.bounds.rows += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qwidget_recalculate(pElement->pWidget, &subRegion);
    if (err) {
      return err;
    }

    // Advance the sub-region to the next widget.
    subRegion.coord.row += boundsIncrement;
    pElement = pElement->pPrevious;

  }

  return 0;
}

//------------------------------------------------------------------------------
static int __qlayout_recalculate_horizontal (
  qlayout_t *                    pLayout,
  qregion_t const *              pRegion
) {
  int err;
  qlayout_element_t * pElement;
  qregion_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.columns / QP(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.columns - (boundsIncrement * QP(pLayout)->widgetCount);
  subRegion = qregion(
    pRegion->coord.column,
    pRegion->coord.row,
    pRegion->bounds.rows,
    boundsIncrement
  );

  // Calculate the region information per-widget
  pElement = QP(pLayout)->pWidgetsFirst;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pNext) {
      subRegion.bounds.columns += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qwidget_recalculate(pElement->pWidget, &subRegion);
    if (err) {
      return err;
    }

    // Advance the sub-region to the next widget.
    subRegion.coord.column += boundsIncrement;
    pElement = pElement->pNext;

  }

  return 0;
}

//------------------------------------------------------------------------------
static int __qlayout_recalculate_horizontal_reverse (
  qlayout_t *                    pLayout,
  qregion_t const *              pRegion
) {
  int err;
  qlayout_element_t * pElement;
  qregion_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.columns / QP(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.columns - (boundsIncrement * QP(pLayout)->widgetCount);
  subRegion = qregion(
    pRegion->coord.column,
    pRegion->coord.row,
    pRegion->bounds.rows,
    boundsIncrement
  );

  // Calculate the region information per-widget
  pElement = QP(pLayout)->pWidgetsLast;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pPrevious) {
      subRegion.bounds.columns += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qwidget_recalculate(pElement->pWidget, &subRegion);
    if (err) {
      return err;
    }

    // Advance the sub-region to the next widget.
    subRegion.coord.column += boundsIncrement;
    pElement = pElement->pPrevious;

  }

  return 0;
}

//------------------------------------------------------------------------------
QRECALC(
  qlayout_recalculate,
  qlayout_t *                    pLayout,
  qregion_t const *              pRegion
) {

  // Calculate the full widget region information.
  QW(pLayout)->contentBounds = pRegion->bounds;
  QW(pLayout)->outerRegion = *pRegion;
  QW(pLayout)->innerRegion = *pRegion;

  switch (QP(pLayout)->layoutFormat) {
    case QLAYOUT_VERTICAL:
      return __qlayout_reacalculate_vertical(
        pLayout,
        pRegion
      );
    case QLAYOUT_VERTICAL_REVERSE:
      return __qlayout_reacalculate_vertical_reverse(
        pLayout,
        pRegion
      );
    case QLAYOUT_HORIZONTAL:
      return __qlayout_recalculate_horizontal(
        pLayout,
        pRegion
      );
    case QLAYOUT_HORIZONTAL_REVERSE:
      return __qlayout_recalculate_horizontal_reverse(
        pLayout,
        pRegion
      );
  }
}

//------------------------------------------------------------------------------
QPAINTER(
  qlayout_paint,
  qlayout_t *                    pLayout,
  qpainter_t *                   pPainter
) {
  int err;
  qlayout_element_t * pElement;

  // Paint all sub-elements of the layout.
  pElement = QP(pLayout)->pWidgetsFirst;
  while (pElement) {
    err = qwidget_paint(pElement->pWidget, pPainter);
    if (err) {
      return err;
    }
    pElement = pElement->pNext;
  }
}

////////////////////////////////////////////////////////////////////////////////
// Layout Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_layout (
  qalloc_t const *               pAllocator,
  qlayout_format_t               format,
  qlayout_t **                   pLayout
) {
  int err;
  qwidget_config_t widgetConfig;
  qlayout_t * layout;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(qlayout_t);
  widgetConfig.privateSize    = sizeof(QPIMPL_STRUCT(qlayout_t));
  widgetConfig.pfnDestroy     = QDESTROY_PTR(qdestroy_layout);
  widgetConfig.pfnRecalculate = QRECALC_PTR(qlayout_recalculate);
  widgetConfig.pfnPaint       = QPAINTER_PTR(qlayout_paint);

  // Allocate the terminal UI application.
  err = qcreate_widget(
    &widgetConfig,
    &layout
  );
  if (err) {
    return err;
  }

  // Set the format for the layout.
  QP(layout)->layoutFormat = format;

  // Return the application to the caller.
  *pLayout = layout;
  return 0;
}

//------------------------------------------------------------------------------
void QCURSESCALL qdestroy_layout (
  qlayout_t *                    pLayout
) {
  // TODO: Add destruction logic.
  qdestroy_widget(pLayout);
}

//------------------------------------------------------------------------------
qlayout_format_t QCURSESCALL qlayout_get_format (
  qlayout_t const *              pLayout
) {
  return QP(pLayout)->layoutFormat;
}

//------------------------------------------------------------------------------
int QCURSESCALL qlayout_set_format (
  qlayout_t const *              pLayout,
  qlayout_format_t               format
) {
  if (QP(pLayout)->layoutFormat != format) {
    qwidget_mark_dirty(pLayout);
    qwidget_emit(pLayout, set_format, format);
  }
  QP(pLayout)->layoutFormat = format;
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL __qlayout_add_widget (
  qlayout_t *                    pLayout,
  qwidget_t *                    pWidget,
  int                                   stretch
) {
  qlayout_element_t * pElement;

  // Allocate the element so that we can attach it.
  pElement = (qlayout_element_t *)qallocate(
    QW(pLayout)->pAllocator,
    sizeof(qlayout_element_t),
    1
  );
  if (!pElement) {
    return ENOMEM;
  }

  // Initialize the important data members.
  pElement->pWidget = pWidget;
  pElement->stretch = stretch;
  pElement->pPrevious = NULL;
  pElement->pNext = NULL;

  // Attach the widget to the layout list.
  if (!QP(pLayout)->pWidgetsFirst) {
    QP(pLayout)->pWidgetsFirst = pElement;
    QP(pLayout)->pWidgetsLast = pElement;
  }
  else {
    pElement->pPrevious = QP(pLayout)->pWidgetsLast;
    pElement->pPrevious->pNext = pElement;
    QP(pLayout)->pWidgetsLast = pElement;
  }

  ++QP(pLayout)->widgetCount;
  QW(pWidget)->pParent = (qwidget_t *)pLayout;
  qwidget_mark_dirty(pLayout);
  return 0;
}