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
#ifndef   QCURSES_WIDGET_H
#define   QCURSES_WIDGET_H

#include "qcurses.h"
#include "array.h"
#include "math.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: Utilize sizePolicy, minSize, maxSize - they sort of exist, but aren't used.
////////////////////////////////////////////////////////////////////////////////
// Widget Definitions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define QCURSES_WIDGET_BEGIN(name, baseType)  typedef struct name name;         \
                                              struct QCURSES_PIMPL_NAME(name);  \
                                              struct name {                     \
                                                baseType base;                  \
                                                struct QCURSES_PIMPL_NAME(name) * pImpl;
#define QCURSES_WIDGET_END                    };

//------------------------------------------------------------------------------
#define QCURSES_WIDGET_PUBLIC(defs)             struct { defs } impl;

//------------------------------------------------------------------------------
#define QCURSES_WIDGET_SIGNALS(defs)            struct { defs } signals;

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_CONNECTION(slotDecl)                                     \
  struct {                                                                      \
    qcurses_widget_t * pSource;                                                 \
    qcurses_widget_t * pTarget;                                                 \
    qcurses_signal_t * pSignal;                                                 \
    slotDecl;                                                                   \
  }

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_SIGNAL(params)                                           \
  QCURSES_DEFINE_ARRAY(                                                         \
    QCURSES_DEFINE_CONNECTION(int (QCURSESPTR *pfnSlot) params) *               \
  )

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_SLOT(name, params, qparams)                              \
  typedef int (QCURSESPTR *QCURSES_SLOT_NAME(name)) params;                     \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_RECALC(name, params, qparams)                           \
  typedef int (QCURSESPTR *QCURSES_RECALC_NAME(name)) params;                  \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QCURSES_DEFINE_PAINTER(name, params, qparams)                           \
  typedef int (QCURSESPTR *QCURSES_PAINTER_NAME(name)) params;                  \
  int QCURSESCALL name qparams

//------------------------------------------------------------------------------
#define QCURSES_SIGNAL(name, ...) QCURSES_DEFINE_SIGNAL((qcurses_widget_t *, __VA_ARGS__)) name
#define QCURSES_SIGNAL_VOID(name) QCURSES_DEFINE_SIGNAL((qcurses_widget_t *)) name

//------------------------------------------------------------------------------
#define QCURSES_SLOT(name, this, ...) QCURSES_DEFINE_SLOT(name, (qcurses_widget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QCURSES_SLOT_VOID(name, this) QCURSES_DEFINE_SLOT(name, (qcurses_widget_t *), (this))

//------------------------------------------------------------------------------
#define QCURSES_DESTROY_PTR(name) ((qcurses_widget_destroy_pfn)&name)

//------------------------------------------------------------------------------
#define QCURSES_RECALC(name, this, ...) QCURSES_DEFINE_RECALC(name, (qcurses_widget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QCURSES_RECALC_PTR(name) ((QCURSES_RECALC_NAME(name))&name)

//------------------------------------------------------------------------------
#define QCURSES_PAINTER(name, this, ...) QCURSES_DEFINE_PAINTER(name, (qcurses_widget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define QCURSES_PAINTER_PTR(name) ((QCURSES_PAINTER_NAME(name))&name)

////////////////////////////////////////////////////////////////////////////////
// Widget Structures
////////////////////////////////////////////////////////////////////////////////

struct qcurses_connection_t;
typedef QCURSES_DEFINE_ARRAY(struct qcurses_connection_t *) qcurses_signal_t;
typedef QCURSES_DEFINE_CONNECTION(void const * pfnSlot) qcurses_connection_t;
typedef QCURSES_DEFINE_ARRAY(qcurses_widget_t *)     qcurses_array_widget_t;
typedef QCURSES_DEFINE_ARRAY(qcurses_connection_t *) qcurses_array_connection_t;

typedef void (QCURSESPTR *qcurses_widget_destroy_pfn)(qcurses_widget_t *);
typedef int (QCURSESPTR *qcurses_widget_recalc_pfn)(qcurses_widget_t *, qcurses_region_t const *);
typedef int (QCURSESPTR *qcurses_widget_paint_pfn)(qcurses_widget_t *, qcurses_painter_t *);

//------------------------------------------------------------------------------
struct qcurses_widget_config_t {
  qcurses_alloc_t const *               pAllocator;
  size_t                                publicSize;
  size_t                                privateSize;
  qcurses_widget_destroy_pfn            pfnDestroy;
  qcurses_widget_recalc_pfn             pfnRecalculate;
  qcurses_widget_paint_pfn              pfnPaint;
};

//------------------------------------------------------------------------------
struct qcurses_widget_t {
  qcurses_alloc_t const *               pAllocator;
  qcurses_widget_t *                    pParent;
  qcurses_state_t                       internalState;
  qcurses_array_connection_t            connections;
  qcurses_policy_t                      sizePolicy;
  qcurses_bounds_t                      minimumBounds;  // Minimum allowed bounds.
  qcurses_bounds_t                      maximumBounds;  // Maximum allowed bounds.
  qcurses_bounds_t                      contentBounds;  // Calculated content bounds (not always limited by min/max).
  qcurses_region_t                      innerRegion;    // The total region that contains content.
  qcurses_region_t                      outerRegion;    // The total region that the widget paints to.
  qcurses_widget_destroy_pfn            pfnDestroy;
  qcurses_widget_recalc_pfn             pfnRecalculate;
  qcurses_widget_paint_pfn              pfnPaint;
};

//------------------------------------------------------------------------------
struct qcurses_widget_parent_t {
  qcurses_widget_t                      base;
  qcurses_array_widget_t                children;
};

////////////////////////////////////////////////////////////////////////////////
// Widget Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_create_widget (
  qcurses_widget_config_t const *       pConfig,
  qcurses_widget_t **                   pWidget
);

//------------------------------------------------------------------------------
#define qcurses_create_widget(pConfig, pWidget)                                 \
  __qcurses_create_widget(                                                      \
    pConfig,                                                                    \
    (qcurses_widget_t **)(pWidget)                                              \
  )

//------------------------------------------------------------------------------
void QCURSESCALL __qcurses_destroy_widget (
  qcurses_widget_t *                    pWidget
);

//------------------------------------------------------------------------------
#define qcurses_destroy_widget(pWidget)                                         \
  __qcurses_destroy_widget(                                                     \
    (qcurses_widget_t *)(pWidget)                                               \
  )

//------------------------------------------------------------------------------
static inline int QCURSESCALL __qcurses_widget_recalculate (
  qcurses_widget_t *                    pWidget,
  qcurses_region_t const *              pRegion
) {
  return pWidget->pfnRecalculate(pWidget, pRegion);
}

//------------------------------------------------------------------------------
#define qcurses_widget_recalculate(pWidget, pRegion)                            \
  __qcurses_widget_recalculate(                                                 \
    (qcurses_widget_t *)(pWidget),                                              \
    pRegion                                                                     \
  )

//------------------------------------------------------------------------------
static inline int QCURSESCALL __qcurses_widget_paint (
  qcurses_widget_t *                    pWidget,
  qcurses_painter_t *                   pPainters
) {
  return pWidget->pfnPaint(pWidget, pPainters);
}

//------------------------------------------------------------------------------
#define qcurses_widget_paint(pWidget, pPainter)                                 \
  __qcurses_widget_paint(                                                       \
    (qcurses_widget_t *)(pWidget),                                              \
    pPainter                                                                    \
  )

//------------------------------------------------------------------------------
int QCURSESCALL __qcurses_widget_prepare_connection (
  qcurses_widget_t *                    pSource,
  qcurses_widget_t *                    pTarget,
  qcurses_signal_t *                    pSignal
);

//------------------------------------------------------------------------------
#define qcurses_widget_connect(pSource, signal, pTarget, slot)                  \
  (                                                                             \
    __qcurses_widget_prepare_connection(                                        \
      (qcurses_widget_t *)(pSource),                                            \
      (qcurses_widget_t *)(pTarget),                                            \
      (qcurses_signal_t *)&(pSource)->signals.signal                            \
    ) ||                                                                        \
    !((pSource)->signals.signal.pData[                                          \
      (pSource)->signals.signal.count - 1                                       \
    ]->pfnSlot = (QCURSES_SLOT_NAME(slot))&slot)                                \
  ) ? (                                                                         \
    (                                                                           \
      ((pSource)->signals.signal.count == QCURSES_SIGNAL_MAX) ||                \
      (((qcurses_widget_t *)(pTarget))->connections.count == QCURSES_SIGNAL_MAX)\
    ) ? ERANGE : ENOMEM                                                         \
  ) : 0

//------------------------------------------------------------------------------
#define qcurses_widget_mark_state(pWidget, state)                               \
  ((((qcurses_widget_t *)(pWidget)))->internalState |= (state))

//------------------------------------------------------------------------------
#define qcurses_widget_unmark_state(pWidget, state)                             \
  ((((qcurses_widget_t *)(pWidget)))->internalState &= ~(state))

//------------------------------------------------------------------------------
#define qcurses_widget_check_state(pWidget, state)                              \
  ((((qcurses_widget_t *)(pWidget)))->internalState & (state))

//------------------------------------------------------------------------------
#define qcurses_widget_set_state(pWidget, state, boolean)                       \
  (                                                                             \
    (boolean) ?                                                                 \
      qcurses_widget_mark_state(pWidget, state) :                               \
      qcurses_widget_unmark_state(pWidget, state)                               \
  )

//------------------------------------------------------------------------------
#define qcurses_widget_toggle_state(pWidget, state)                             \
  qcurses_widget_set_state(                                                     \
    pWidget,                                                                    \
    state,                                                                      \
    !qcurses_widget_check_state(pWidget, state)                                 \
  )

//------------------------------------------------------------------------------
#define qcurses_widget_is_visible(pWidget)                                      \
  qcurses_widget_check_state(pWidget, QCURSES_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_mark_visible(pWidget)                                    \
  qcurses_widget_mark_state(pWidget, QCURSES_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_unmark_visible(pWidget)                                  \
  qcurses_widget_unmark_state(pWidget, QCURSES_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_set_visible(pWidget, boolean)                            \
  qcurses_widget_set_state(pWidget, QCURSES_STATE_VISIBLE_BIT, boolean)

//------------------------------------------------------------------------------
#define qcurses_widget_toggle_visible(pWidget)                                  \
  qcurses_widget_toggle_state(pWidget, QCURSES_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_is_dirty(pWidget)                                        \
  qcurses_widget_check_state(pWidget, QCURSES_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
qcurses_state_t __qcurses_widget_mark_dirty (
  qcurses_widget_t *                    pWidget
);

//------------------------------------------------------------------------------
#define qcurses_widget_mark_dirty(pWidget)                                      \
  __qcurses_widget_mark_dirty((qcurses_widget_t *)pWidget)

//------------------------------------------------------------------------------
#define qcurses_widget_unmark_dirty(pWidget)                                    \
  qcurses_widget_unmark_state(pWidget, QCURSES_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_set_dirty(pWidget, boolean)                              \
  qcurses_widget_set_state(pWidget, QCURSES_STATE_DIRTY_BIT, boolean)

//------------------------------------------------------------------------------
#define qcurses_widget_toggle_dirty(pWidget)                                    \
  qcurses_widget_toggle_state(pWidget, QCURSES_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define qcurses_widget_check(pWidget)                                           \
  ((pWidget) && qcurses_widget_is_visible(pWidget))

//------------------------------------------------------------------------------
#define qcurses_widget_emit(pWidget, signal, ...)                               \
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

#endif // QCURSES_WIDGET_H
