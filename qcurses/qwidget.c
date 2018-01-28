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

#include "qwidget.h"
#include <string.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Widget Helpers
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
typedef struct qwidget_pimpl_t {
  qwidget_t                           baseWidget;
  void *                              pImpl;
} qwidget_pimpl_t;

////////////////////////////////////////////////////////////////////////////////
// Widget Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qcreate_widget (
  qwidget_config_t const *              pConfig,
  qwidget_t **                          pResult
) {
  size_t totalSize;
  qwidget_pimpl_t * widget;
  qalloc_t const * pAllocator;

  // Ensure that we are setting a valid allocator.
  pAllocator = pConfig->pAllocator;
  if (!pAllocator) {
    pAllocator = qdefault_allocator();
  }

  // Allocate the widget of the desired size.
  totalSize = pConfig->publicSize + pConfig->privateSize;
  widget = (qwidget_pimpl_t *)qallocate_unsafe(
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
  widget->baseWidget.pAllocator       = pAllocator;
  widget->baseWidget.pParent          = NULL;
  widget->baseWidget.pfnDestroy       = pConfig->pfnDestroy;
  widget->baseWidget.pfnRecalculate   = pConfig->pfnRecalculate;
  widget->baseWidget.pfnPaint         = pConfig->pfnPaint;
  widget->baseWidget.minimumBounds    = qbounds(0, 0);
  widget->baseWidget.maximumBounds    = qbounds(QINFINITE, QINFINITE);
  widget->baseWidget.sizePolicy       = QPOLICY_PREFERRED;
  widget->baseWidget.internalState    = QSTATE_DIRTY_BIT | QSTATE_ENABLED_BIT | QSTATE_VISIBLE_BIT;
  widget->pImpl = ((char *)widget) + pConfig->publicSize;

  *pResult = &widget->baseWidget;
  return 0;
}

//------------------------------------------------------------------------------
void QCURSESCALL __qdestroy_widget (
  qwidget_t *                           pWidget
) {
  pWidget->pfnDestroy(pWidget);
}

//------------------------------------------------------------------------------
int QCURSESCALL __qwidget_prepare_connection (
  qwidget_t *                           pSource,
  qwidget_t *                           pTarget,
  qsignal_t *                           pSignal
) {
  int err;
  qconnection_t * pConnection;

  // Grow the signals/slots arrays if needed.
  err = qarray_ensure(pSource->pAllocator, pSignal);
  if (err) {
    return err;
  }
  err = qarray_ensure(pTarget->pAllocator, &pTarget->connections);
  if (err) {
    return err;
  }

  // Construct a new connection (owned by target).
  pConnection = qallocate(
    pTarget->pAllocator,
    sizeof(qconnection_t),
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
  (void)qarray_push(pSource->pAllocator, pSignal, pConnection);
  (void)qarray_push(pTarget->pAllocator, &pTarget->connections, pConnection);

  return 0;
}

//------------------------------------------------------------------------------
qstate_t QCURSESCALL __qwidget_mark_dirty (
  qwidget_t *                           pWidget
) {
  qstate_t newState;

  // Mark the state of the current widget as dirty,
  // We return the widget state simply to adhere to other mark_*() return values.
  qwidget_mark_state(pWidget, QSTATE_DIRTY_BIT);
  newState = pWidget->internalState;

  // Iteratively mark the parents recursive until we find a dirty parent.
  // This allows the update chain to know that it needs to be refreshed.
  while ((pWidget = pWidget->pParent)) {
    if (qwidget_check_state(pWidget, QSTATE_DIRTY_BIT)) {
      break;
    }
    qwidget_mark_state(pWidget, QSTATE_DIRTY_BIT);
  }

  return newState;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
