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
 * Slate Terminal UI Source
 ******************************************************************************/

#include "widget.h"
#include <string.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Public Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int __slate_signal_resize (
  slate_widget_t *                      pWidget,
  slate_signal_t *                      pSignal,
  uint32_t                              newCapacity
) {

  // Signals may not arbitrarily remove elements.
  if (newCapacity < pSignal->count) {
    return EINVAL;
  }

  return slate_array_resize(pWidget->pAllocator, pSignal, newCapacity);
}

//------------------------------------------------------------------------------
static int __slate_signal_grow (
  slate_widget_t *                      pWidget,
  slate_signal_t *                      pSignal
) {
  return slate_array_grow(pWidget->pAllocator, pSignal);
}

//------------------------------------------------------------------------------
static int __slate_slots_resize (
  slate_widget_t *                      pWidget,
  uint32_t                              newCapacity
) {

  // Slots may not arbitrarily remove elements.
  if (newCapacity < pWidget->connections.count) {
    return EINVAL;
  }

  return slate_array_resize(pWidget->pAllocator, &pWidget->connections, newCapacity);
}

//------------------------------------------------------------------------------
static int __slate_slots_grow (
  slate_widget_t *                      pWidget
) {
  return slate_array_grow(pWidget->pAllocator, &pWidget->connections);
}

//------------------------------------------------------------------------------
int __slate_create_widget (
  slate_widget_config_t const *         pConfig,
  slate_widget_t **                     pResult
) {
  slate_widget_t * pWidget;

  // Allocate the widget of the desired size.
  pWidget = slate_allocate(
    pConfig->pAllocator,
    pConfig->widgetSize,
    1
  );
  if (!pWidget) {
    return ENOMEM;
  }

  // Zeroing out the data will set most of the widget to reasonable defaults.
  memset(pWidget, 0, pConfig->widgetSize);

  // Initialize the fields for the widget.
  pWidget->pAllocator = pConfig->pAllocator;
  pWidget->pParent = NULL;
  pWidget->pfnDestroy = pConfig->pfnDestroy;
  pWidget->pfnRecalculate = pConfig->pfnRecalculate;
  pWidget->pfnPaint = pConfig->pfnPaint;
  pWidget->minimumBounds = slate_bounds(0, 0);
  pWidget->maximumBounds = slate_bounds(SLATE_INFINITE, SLATE_INFINITE);
  pWidget->sizePolicy = SLATE_POLICY_PREFERRED;
  pWidget->internalState = SLATE_STATE_DIRTY_BIT | SLATE_STATE_VISIBLE_BIT;

  *pResult = pWidget;
  return 0;
}

//------------------------------------------------------------------------------
void __slate_destroy_widget (
  slate_widget_t *                      pWidget
) {
  pWidget->pfnDestroy(pWidget);
}

//------------------------------------------------------------------------------
int __slate_widget_prepare_connection (
  slate_widget_t *                      pSource,
  slate_widget_t *                      pTarget,
  slate_signal_t *                      pSignal
) {
  int err;
  slate_connection_t * pConnection;

  // Grow the signals/slots arrays if needed.
  err = slate_array_ensure(pSource->pAllocator, pSignal);
  if (err) {
    return err;
  }
  err = slate_array_ensure(pTarget->pAllocator, &pTarget->connections);
  if (err) {
    return err;
  }

  // Construct a new connection (owned by target).
  pConnection = slate_allocate(
    pTarget->pAllocator,
    sizeof(slate_connection_t),
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
  (void)slate_array_push(pSource->pAllocator, pSignal, pConnection);
  (void)slate_array_push(pTarget->pAllocator, &pTarget->connections, pConnection);

  return 0;
}

//------------------------------------------------------------------------------
// TODO: Do we need an iterative parent mark_dirty function?
slate_state_t __slate_widget_mark_dirty (
  slate_widget_t *                      pWidget
) {
  slate_state_t newState;

  // Mark the state of the current widget as dirty,
  // We return the widget state simply to adhere to other mark_*() return values.
  slate_widget_mark_state(pWidget, SLATE_STATE_DIRTY_BIT);
  newState = pWidget->internalState;

  // Iteratively mark the parents recursive until we find a dirty parent.
  // This allows the update chain to know that it needs to be refreshed.
  while ((pWidget = pWidget->pParent)) {
    if (slate_widget_check_state(pWidget, SLATE_STATE_DIRTY_BIT)) {
      break;
    }
    slate_widget_mark_state(pWidget, SLATE_STATE_DIRTY_BIT);
  }

  return newState;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
