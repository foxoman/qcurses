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
#ifndef   QWIDGET_H
#define   QWIDGET_H

#include "qcurses.h"
#include "qarray.h"
#include "qmath.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: Utilize sizePolicy, minSize, maxSize - they sort of exist, but aren't used.
////////////////////////////////////////////////////////////////////////////////
// Widget Definitions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define QWIDGET_BEGIN(name)                                                     \
  typedef struct name name;                                                     \
    struct QPIMPL_NAME(name);                                                   \
    struct name {                                                               \
      qwidget_t base;                                                           \
      struct QPIMPL_NAME(name) * pImpl;
#define QWIDGET_END };

//------------------------------------------------------------------------------
#define QWIDGET_PUBLICS_BEGIN struct {
#define QWIDGET_PUBLICS_END   } impl;

//------------------------------------------------------------------------------
#define QWIDGET_SIGNALS_BEGIN struct {
#define QWIDGET_SIGNALS_END   } signals;

//------------------------------------------------------------------------------
#define QDEFINE_CONNECTION(slotDecl)                                            \
  struct {                                                                      \
    qwidget_t * pSource;                                                        \
    qwidget_t * pTarget;                                                        \
    qsignal_t * pSignal;                                                        \
    slotDecl;                                                                   \
  }

//------------------------------------------------------------------------------
#define QDEFINE_SIGNAL(params)                                                  \
  QDEFINE_ARRAY(                                                                \
    QDEFINE_CONNECTION(int (QCURSESPTR *pfnSlot) params) *                      \
  )

//------------------------------------------------------------------------------
#define QDEFINE_SLOT(name, params, qparams)                                     \
  typedef int (QCURSESPTR *QSLOT_NAME(name)) params;                            \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QDEFINE_RECALC(name, params, qparams)                                   \
  typedef int (QCURSESPTR *QRECALC_NAME(name)) params;                          \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QDEFINE_PAINTER(name, params, qparams)                                  \
  typedef int (QCURSESPTR *QPAINTER_NAME(name)) params;                         \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QSIGNAL(name, ...) QDEFINE_SIGNAL((qwidget_t *, __VA_ARGS__)) name
#define QSIGNAL_VOID(name) QDEFINE_SIGNAL((qwidget_t *)) name

//------------------------------------------------------------------------------
#define QSLOT(name, this, ...) QDEFINE_SLOT(name, (qwidget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QSLOT_VOID(name, this) QDEFINE_SLOT(name, (qwidget_t *), (this))

//------------------------------------------------------------------------------
#define QDESTROY_PTR(name) ((qwidget_destroy_pfn)&name)

//------------------------------------------------------------------------------
#define QRECALC(name, this, ...) QDEFINE_RECALC(name, (qwidget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QRECALC_PTR(name) ((QRECALC_NAME(name))&name)

//------------------------------------------------------------------------------
#define QPAINTER(name, this, ...) QDEFINE_PAINTER(name, (qwidget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QPAINTER_PTR(name) ((QPAINTER_NAME(name))&name)

//------------------------------------------------------------------------------
#define QP(pointer) ((pointer)->pImpl)
#define QW(pointer) ((qwidget_t *)(pointer))

////////////////////////////////////////////////////////////////////////////////
// Widget Structures
////////////////////////////////////////////////////////////////////////////////

struct qconnection_t;
typedef QDEFINE_ARRAY(struct qconnection_t *)     qsignal_t;
typedef QDEFINE_CONNECTION(void const * pfnSlot)  qconnection_t;
typedef QDEFINE_ARRAY(qwidget_t *)                qarray_widget_t;
typedef QDEFINE_ARRAY(qconnection_t *)            qarray_connection_t;

typedef void (QCURSESPTR *qwidget_destroy_pfn)(qwidget_t *);
typedef int (QCURSESPTR *qwidget_recalc_pfn)(qwidget_t *, qregion_t const *);
typedef int (QCURSESPTR *qwidget_paint_pfn)(qwidget_t *, qpainter_t *);

//------------------------------------------------------------------------------
struct qwidget_config_t {
  qalloc_t const *                      pAllocator;
  size_t                                publicSize;
  size_t                                privateSize;
  qwidget_destroy_pfn                   pfnDestroy;
  qwidget_recalc_pfn                    pfnRecalculate;
  qwidget_paint_pfn                     pfnPaint;
};

//------------------------------------------------------------------------------
struct qwidget_t {
  qalloc_t const *                      pAllocator;
  qwidget_t *                           pParent;
  qstate_t                              internalState;
  qarray_connection_t                   connections;
  qpolicy_t                             sizePolicy;
  qbounds_t                             minimumBounds;  // Minimum allowed bounds.
  qbounds_t                             maximumBounds;  // Maximum allowed bounds.
  qbounds_t                             contentBounds;  // Calculated content bounds (not always limited by min/max).
  qregion_t                             innerRegion;    // The total region that contains content.
  qregion_t                             outerRegion;    // The total region that the widget paints to.
  qwidget_destroy_pfn                   pfnDestroy;
  qwidget_recalc_pfn                    pfnRecalculate;
  qwidget_paint_pfn                     pfnPaint;
};

////////////////////////////////////////////////////////////////////////////////
// Widget Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qcreate_widget (
  qwidget_config_t const *              pConfig,
  qwidget_t **                          pWidget
);

//------------------------------------------------------------------------------
#define qcreate_widget(pConfig, pWidget)                                        \
  __qcreate_widget(                                                             \
    pConfig,                                                                    \
    (qwidget_t **)(pWidget)                                                     \
  )

//------------------------------------------------------------------------------
void QCURSESCALL __qdestroy_widget (
  qwidget_t *                           pWidget
);

//------------------------------------------------------------------------------
#define qdestroy_widget(pWidget)                                                \
  __qdestroy_widget(                                                            \
    (qwidget_t *)(pWidget)                                                      \
  )

//------------------------------------------------------------------------------
static inline int QCURSESCALL __qwidget_recalculate (
  qwidget_t *                           pWidget,
  qregion_t const *                     pRegion
) {
  return pWidget->pfnRecalculate(pWidget, pRegion);
}

//------------------------------------------------------------------------------
#define qwidget_recalculate(pWidget, pRegion)                                   \
  __qwidget_recalculate(                                                        \
    (qwidget_t *)(pWidget),                                                     \
    pRegion                                                                     \
  )

//------------------------------------------------------------------------------
static inline int QCURSESCALL __qwidget_paint (
  qwidget_t *                           pWidget,
  qpainter_t *                          pPainters
) {
  return pWidget->pfnPaint(pWidget, pPainters);
}

//------------------------------------------------------------------------------
#define qwidget_paint(pWidget, pPainter)                                        \
  __qwidget_paint(                                                              \
    (qwidget_t *)(pWidget),                                                     \
    pPainter                                                                    \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qwidget_prepare_connection (
  qwidget_t *                           pSource,
  qwidget_t *                           pTarget,
  qsignal_t *                           pSignal
);

//------------------------------------------------------------------------------
#define qwidget_connect(pSource, signal, pTarget, slot)                         \
  (                                                                             \
    __qwidget_prepare_connection(                                               \
      (qwidget_t *)(pSource),                                                   \
      (qwidget_t *)(pTarget),                                                   \
      (qsignal_t *)&(pSource)->signals.signal                                   \
    ) ||                                                                        \
    !((pSource)->signals.signal.pData[                                          \
      (pSource)->signals.signal.count - 1                                       \
    ]->pfnSlot = (QSLOT_NAME(slot))&slot)                                       \
  ) ? (                                                                         \
    (                                                                           \
      ((pSource)->signals.signal.count == QSIGNAL_MAX) ||                       \
      (((qwidget_t *)(pTarget))->connections.count == QSIGNAL_MAX)              \
    ) ? ERANGE : ENOMEM                                                         \
  ) : 0

//------------------------------------------------------------------------------
#define qwidget_mark_state(pWidget, state)                                      \
  ((((qwidget_t *)(pWidget)))->internalState |= (state))

//------------------------------------------------------------------------------
#define qwidget_unmark_state(pWidget, state)                                    \
  ((((qwidget_t *)(pWidget)))->internalState &= ~(state))

//------------------------------------------------------------------------------
#define qwidget_check_state(pWidget, state)                                     \
  ((((qwidget_t *)(pWidget)))->internalState & (state))

//------------------------------------------------------------------------------
#define qwidget_set_state(pWidget, state, boolean)                              \
  (                                                                             \
    (boolean) ?                                                                 \
      qwidget_mark_state(pWidget, state) :                                      \
      qwidget_unmark_state(pWidget, state)                                      \
  )

//------------------------------------------------------------------------------
#define qwidget_toggle_state(pWidget, state)                                    \
  qwidget_set_state(                                                            \
    pWidget,                                                                    \
    state,                                                                      \
    !qwidget_check_state(pWidget, state)                                        \
  )

//------------------------------------------------------------------------------
#define qwidget_is_visible(pWidget)                                             \
  qwidget_check_state(pWidget, QSTATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qwidget_mark_visible(pWidget)                                           \
  qwidget_mark_state(pWidget, QSTATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qwidget_unmark_visible(pWidget)                                         \
  qwidget_unmark_state(pWidget, QSTATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qwidget_set_visible(pWidget, boolean)                                   \
  qwidget_set_state(pWidget, QSTATE_VISIBLE_BIT, boolean)

//------------------------------------------------------------------------------
#define qwidget_toggle_visible(pWidget)                                         \
  qwidget_toggle_state(pWidget, QSTATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qwidget_is_dirty(pWidget)                                               \
  qwidget_check_state(pWidget, QSTATE_DIRTY_BIT)

//------------------------------------------------------------------------------
qstate_t QCURSESCALL __qwidget_mark_dirty (
  qwidget_t *                    pWidget
);

//------------------------------------------------------------------------------
#define qwidget_mark_dirty(pWidget)                                             \
  __qwidget_mark_dirty((qwidget_t *)pWidget)

//------------------------------------------------------------------------------
#define qwidget_unmark_dirty(pWidget)                                           \
  qwidget_unmark_state(pWidget, QSTATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define qwidget_set_dirty(pWidget, boolean)                                     \
  qwidget_set_state(pWidget, QSTATE_DIRTY_BIT, boolean)

//------------------------------------------------------------------------------
#define qwidget_toggle_dirty(pWidget)                                           \
  qwidget_toggle_state(pWidget, QSTATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define qwidget_check(pWidget)                                                  \
  ((pWidget) && qwidget_is_visible(pWidget))

//------------------------------------------------------------------------------
#define qwidget_emit(pWidget, signal, ...)                                      \
  for (uint32_t __idx = 0; __idx < (pWidget)->signals.signal.count; ++__idx) {  \
    int __result = (pWidget)->signals.signal.pData[__idx]->pfnSlot(             \
      (pWidget)->signals.signal.pData[__idx]->pTarget,                          \
      __VA_ARGS__                                                               \
    );                                                                          \
    if (__result) return __result;                                              \
  }

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QWIDGET_H
