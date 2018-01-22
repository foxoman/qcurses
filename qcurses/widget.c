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

#include "widget.h"
#include <string.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct qcurses_widget_pimpl_t {
  qcurses_widget_t                    baseWidget;
  void *                              pImpl;
} qcurses_widget_pimpl_t;

////////////////////////////////////////////////////////////////////////////////
// Widget Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int __qcurses_signal_resize (
  qcurses_widget_t *                    pWidget,
  qcurses_signal_t *                    pSignal,
  uint32_t                              newCapacity
) {

  // Signals may not arbitrarily remove elements.
  if (newCapacity < pSignal->count) {
    return EINVAL;
  }

  return qcurses_array_resize(pWidget->pAllocator, pSignal, newCapacity);
}

//------------------------------------------------------------------------------
static int __qcurses_signal_grow (
  qcurses_widget_t *                    pWidget,
  qcurses_signal_t *                    pSignal
) {
  return qcurses_array_grow(pWidget->pAllocator, pSignal);
}

//------------------------------------------------------------------------------
static int __qcurses_slots_resize (
  qcurses_widget_t *                    pWidget,
  uint32_t                              newCapacity
) {

  // Slots may not arbitrarily remove elements.
  if (newCapacity < pWidget->connections.count) {
    return EINVAL;
  }

  return qcurses_array_resize(pWidget->pAllocator, &pWidget->connections, newCapacity);
}

//------------------------------------------------------------------------------
static int __qcurses_slots_grow (
  qcurses_widget_t *                    pWidget
) {
  return qcurses_array_grow(pWidget->pAllocator, &pWidget->connections);
}

//------------------------------------------------------------------------------
int __qcurses_create_widget (
  qcurses_widget_config_t const *       pConfig,
  qcurses_widget_t **                   pResult
) {
  size_t totalSize;
  qcurses_widget_pimpl_t * widget;
  qcurses_alloc_t const * pAllocator;

  // Ensure that we are setting a valid allocator.
  pAllocator = pConfig->pAllocator;
  if (!pAllocator) {
    pAllocator = qcurses_default_allocator();
  }

  // Allocate the widget of the desired size.
  totalSize = pConfig->publicSize + pConfig->privateSize;
  widget = (qcurses_widget_pimpl_t *)qcurses_allocate_unsafe(
    pAllocator,
    totalSize,
    1
  );
  if (!widget) {
    return ENOMEM;
  }

  // Zeroing out the data will set most of the widget to reasonable defaults.
  memset(widget, 0, totalSize);

  // Initialize the fields for the widget.
  widget->baseWidget.pAllocator = pAllocator;
  widget->baseWidget.pParent = NULL;
  widget->baseWidget.pfnDestroy = pConfig->pfnDestroy;
  widget->baseWidget.pfnRecalculate = pConfig->pfnRecalculate;
  widget->baseWidget.pfnPaint = pConfig->pfnPaint;
  widget->baseWidget.minimumBounds = qcurses_bounds(0, 0);
  widget->baseWidget.maximumBounds = qcurses_bounds(QCURSES_INFINITE, QCURSES_INFINITE);
  widget->baseWidget.sizePolicy = QCURSES_POLICY_PREFERRED;
  widget->baseWidget.internalState = QCURSES_STATE_DIRTY_BIT | QCURSES_STATE_VISIBLE_BIT;
  widget->pImpl = ((char *)widget) + pConfig->publicSize;

  *pResult = &widget->baseWidget;
  return 0;
}

//------------------------------------------------------------------------------
void __qcurses_destroy_widget (
  qcurses_widget_t *                    pWidget
) {
  pWidget->pfnDestroy(pWidget);
}

//------------------------------------------------------------------------------
int __qcurses_widget_prepare_connection (
  qcurses_widget_t *                    pSource,
  qcurses_widget_t *                    pTarget,
  qcurses_signal_t *                    pSignal
) {
  int err;
  qcurses_connection_t * pConnection;

  // Grow the signals/slots arrays if needed.
  err = qcurses_array_ensure(pSource->pAllocator, pSignal);
  if (err) {
    return err;
  }
  err = qcurses_array_ensure(pTarget->pAllocator, &pTarget->connections);
  if (err) {
    return err;
  }

  // Construct a new connection (owned by target).
  pConnection = qcurses_allocate(
    pTarget->pAllocator,
    sizeof(qcurses_connection_t),
    1
  );
  if (!pConnection) {
    return ENOMEM;
  }

  // Configure the connection
  pConnection->pSource = pSource;
  pConnection->pTarget = pTarget;
  pConnection->pSignal = pSignal;
  pConnection->pfnSlot = NULL;

  // Attach the connection to the signal/slot.
  // Note: No need to check the result, since we already ensured capacity.
  (void)qcurses_array_push(pSource->pAllocator, pSignal, pConnection);
  (void)qcurses_array_push(pTarget->pAllocator, &pTarget->connections, pConnection);

  return 0;
}

//------------------------------------------------------------------------------
qcurses_state_t __qcurses_widget_mark_dirty (
  qcurses_widget_t *                    pWidget
) {
  qcurses_state_t newState;

  // Mark the state of the current widget as dirty,
  // We return the widget state simply to adhere to other mark_*() return values.
  qcurses_widget_mark_state(pWidget, QCURSES_STATE_DIRTY_BIT);
  newState = pWidget->internalState;

  // Iteratively mark the parents recursive until we find a dirty parent.
  // This allows the update chain to know that it needs to be refreshed.
  while ((pWidget = pWidget->pParent)) {
    if (qcurses_widget_check_state(pWidget, QCURSES_STATE_DIRTY_BIT)) {
      break;
    }
    qcurses_widget_mark_state(pWidget, QCURSES_STATE_DIRTY_BIT);
  }

  return newState;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
