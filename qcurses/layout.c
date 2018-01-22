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

#include "layout.h"

// TODO: Support element stretching, margins, layout properties.
////////////////////////////////////////////////////////////////////////////////
// Layout Implementations
////////////////////////////////////////////////////////////////////////////////

struct qcurses_layout_element_t;
typedef struct qcurses_layout_element_t {
  struct qcurses_layout_element_t *     pNext;
  struct qcurses_layout_element_t *     pPrevious;
  qcurses_widget_t *                    pWidget;
  int                                   stretch;
} qcurses_layout_element_t;

//------------------------------------------------------------------------------
struct QCURSES_PIMPL_NAME(qcurses_layout_t) {
  qcurses_layout_element_t *            pWidgetsFirst;
  qcurses_layout_element_t *            pWidgetsLast;
  qcurses_layout_format_t               layoutFormat;
  size_t                                widgetCount;
};

//------------------------------------------------------------------------------
static int __qcurses_layout_reacalculate_vertical (
  qcurses_layout_t *                    pLayout,
  qcurses_region_t const *              pRegion
) {
  int err;
  qcurses_layout_element_t * pElement;
  qcurses_region_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.rows / P(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.rows - (boundsIncrement * P(pLayout)->widgetCount);
  subRegion = qcurses_region(
    pRegion->coord.column,
    pRegion->coord.row,
    boundsIncrement,
    pRegion->bounds.columns
  );

  // Calculate the region information per-widget
  pElement = P(pLayout)->pWidgetsFirst;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pNext) {
      subRegion.bounds.rows += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qcurses_widget_recalculate(pElement->pWidget, &subRegion);
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
static int __qcurses_layout_reacalculate_vertical_reverse (
  qcurses_layout_t *                    pLayout,
  qcurses_region_t const *              pRegion
) {
  int err;
  qcurses_layout_element_t * pElement;
  qcurses_region_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.rows / P(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.rows - (boundsIncrement * P(pLayout)->widgetCount);
  subRegion = qcurses_region(
    pRegion->coord.column,
    pRegion->coord.row,
    boundsIncrement,
    pRegion->bounds.columns
  );

  // Calculate the region information per-widget
  pElement = P(pLayout)->pWidgetsLast;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pPrevious) {
      subRegion.bounds.rows += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qcurses_widget_recalculate(pElement->pWidget, &subRegion);
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
static int __qcurses_layout_recalculate_horizontal (
  qcurses_layout_t *                    pLayout,
  qcurses_region_t const *              pRegion
) {
  int err;
  qcurses_layout_element_t * pElement;
  qcurses_region_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.columns / P(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.columns - (boundsIncrement * P(pLayout)->widgetCount);
  subRegion = qcurses_region(
    pRegion->coord.column,
    pRegion->coord.row,
    pRegion->bounds.rows,
    boundsIncrement
  );

  // Calculate the region information per-widget
  pElement = P(pLayout)->pWidgetsFirst;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pNext) {
      subRegion.bounds.columns += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qcurses_widget_recalculate(pElement->pWidget, &subRegion);
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
static int __qcurses_layout_recalculate_horizontal_reverse (
  qcurses_layout_t *                    pLayout,
  qcurses_region_t const *              pRegion
) {
  int err;
  qcurses_layout_element_t * pElement;
  qcurses_region_t subRegion;
  size_t boundsIncrement;
  size_t boundsRemaining;

  // Given this information, we can split the widget into regions.
  boundsIncrement = pRegion->bounds.columns / P(pLayout)->widgetCount;
  boundsRemaining = pRegion->bounds.columns - (boundsIncrement * P(pLayout)->widgetCount);
  subRegion = qcurses_region(
    pRegion->coord.column,
    pRegion->coord.row,
    pRegion->bounds.rows,
    boundsIncrement
  );

  // Calculate the region information per-widget
  pElement = P(pLayout)->pWidgetsLast;
  while (pElement) {

    // If this is the last widget, we should add the extraneous bounds.
    if (!pElement->pPrevious) {
      subRegion.bounds.columns += boundsRemaining;
    }

    // Recalculate the next widget in the bunch.
    err = qcurses_widget_recalculate(pElement->pWidget, &subRegion);
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
QCURSES_RECALC(
  qcurses_layout_recalculate,
  qcurses_layout_t *                    pLayout,
  qcurses_region_t const *              pRegion
) {

  // Calculate the full widget region information.
  W(pLayout)->contentBounds = pRegion->bounds;
  W(pLayout)->outerRegion = *pRegion;
  W(pLayout)->innerRegion = *pRegion;

  switch (P(pLayout)->layoutFormat) {
    case QCURSES_LAYOUT_VERTICAL:
      return __qcurses_layout_reacalculate_vertical(
        pLayout,
        pRegion
      );
    case QCURSES_LAYOUT_VERTICAL_REVERSE:
      return __qcurses_layout_reacalculate_vertical_reverse(
        pLayout,
        pRegion
      );
    case QCURSES_LAYOUT_HORIZONTAL:
      return __qcurses_layout_recalculate_horizontal(
        pLayout,
        pRegion
      );
    case QCURSES_LAYOUT_HORIZONTAL_REVERSE:
      return __qcurses_layout_recalculate_horizontal_reverse(
        pLayout,
        pRegion
      );
  }
}

//------------------------------------------------------------------------------
QCURSES_PAINTER(
  qcurses_layout_paint,
  qcurses_layout_t *                    pLayout,
  qcurses_painter_t *                   pPainter
) {
  int err;
  qcurses_layout_element_t * pElement;

  // Paint all sub-elements of the layout.
  pElement = P(pLayout)->pWidgetsFirst;
  while (pElement) {
    err = qcurses_widget_paint(pElement->pWidget, pPainter);
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
int qcurses_create_layout (
  qcurses_alloc_t const *               pAllocator,
  qcurses_layout_format_t               format,
  qcurses_layout_t **                   pLayout
) {
  int err;
  qcurses_widget_config_t widgetConfig;
  qcurses_layout_t * layout;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(qcurses_layout_t);
  widgetConfig.privateSize    = sizeof(QCURSES_PIMPL_STRUCT(qcurses_layout_t));
  widgetConfig.pfnDestroy     = QCURSES_DESTROY_PTR(qcurses_destroy_layout);
  widgetConfig.pfnRecalculate = QCURSES_RECALC_PTR(qcurses_layout_recalculate);
  widgetConfig.pfnPaint       = QCURSES_PAINTER_PTR(qcurses_layout_paint);

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &layout
  );
  if (err) {
    return err;
  }

  // Set the format for the layout.
  P(layout)->layoutFormat = format;

  // Return the application to the caller.
  *pLayout = layout;
  return 0;
}

//------------------------------------------------------------------------------
void qcurses_destroy_layout (
  qcurses_layout_t *                    pLayout
) {
  // TODO: Add destruction logic.
  qcurses_destroy_widget(pLayout);
}

//------------------------------------------------------------------------------
qcurses_layout_format_t qcurses_layout_get_format (
  qcurses_layout_t const *              pLayout
) {
  return P(pLayout)->layoutFormat;
}

//------------------------------------------------------------------------------
int qcurses_layout_set_format (
  qcurses_layout_t const *              pLayout,
  qcurses_layout_format_t               format
) {
  if (P(pLayout)->layoutFormat != format) {
    qcurses_widget_mark_dirty(pLayout);
    qcurses_widget_emit(pLayout, onFormat, format);
  }
  P(pLayout)->layoutFormat = format;
  return 0;
}

//------------------------------------------------------------------------------
int __qcurses_layout_add_widget (
  qcurses_layout_t *                    pLayout,
  qcurses_widget_t *                    pWidget,
  int                                   stretch
) {
  qcurses_layout_element_t * pElement;

  // Allocate the element so that we can attach it.
  pElement = (qcurses_layout_element_t *)qcurses_allocate(
    W(pLayout)->pAllocator,
    sizeof(qcurses_layout_element_t),
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
  if (!P(pLayout)->pWidgetsFirst) {
    P(pLayout)->pWidgetsFirst = pElement;
    P(pLayout)->pWidgetsLast = pElement;
  }
  else {
    pElement->pPrevious = P(pLayout)->pWidgetsLast;
    pElement->pPrevious->pNext = pElement;
    P(pLayout)->pWidgetsLast = pElement;
  }

  ++P(pLayout)->widgetCount;
  W(pWidget)->pParent = (qcurses_widget_t *)pLayout;
  qcurses_widget_mark_dirty(pLayout);
  return 0;
}