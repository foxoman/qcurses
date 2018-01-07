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
 * Slate Terminal UI Include Header
 ******************************************************************************/
#ifndef   SLATE_WIDGET_H
#define   SLATE_WIDGET_H

#include "slate.h"
#include "array.h"
#include "math.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

// TODO: Utilize sizePolicy, minSize, maxSize - they sort of exist, but aren't used.
////////////////////////////////////////////////////////////////////////////////
// Macro Definitions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define SLATE_WIDGET_BEGIN(name, baseType)  typedef struct name name;           \
                                            struct SLATE_PIMPL_NAME(name);      \
                                            struct name {                       \
                                              baseType base;                    \
                                              struct SLATE_PIMPL_NAME(name) * pImpl;
#define SLATE_WIDGET_PUBLIC(defs)             struct { defs } impl;
#define SLATE_WIDGET_SIGNALS(defs)            struct { defs } signals;
#define SLATE_WIDGET_END                    };

//------------------------------------------------------------------------------
#define SLATE_DEFINE_CONNECTION(slotDecl)                                       \
  struct {                                                                      \
    slate_widget_t * pSource;                                                   \
    slate_widget_t * pTarget;                                                   \
    struct slate_signal_t * pSignal;                                            \
    slotDecl;                                                                   \
  }

//------------------------------------------------------------------------------
#define SLATE_DEFINE_SIGNAL(params)                                             \
  SLATE_DEFINE_ARRAY(                                                           \
    SLATE_DEFINE_CONNECTION(int (SLATEPTR *pfnSlot) params) *                   \
  )

//------------------------------------------------------------------------------
#define SLATE_DEFINE_SLOT(name, params, qparams)                                \
  typedef int (SLATEPTR *SLATE_SLOT_NAME(name)) params;                         \
  int SLATECALL name qparams

//------------------------------------------------------------------------------
#define SLATE_SIGNAL(name, ...) SLATE_DEFINE_SIGNAL((slate_widget_t *, __VA_ARGS__)) name
#define SLATE_SIGNAL_VOID(name) SLATE_DEFINE_SIGNAL((slate_widget_t *)) name

//------------------------------------------------------------------------------
#define SLATE_SLOT(name, this, ...) SLATE_DEFINE_SLOT(name, (slate_widget_t *, __VA_ARGS__), (this, __VA_ARGS__))
#define SLATE_SLOT_VOID(name, this) SLATE_DEFINE_SLOT(name, (slate_widget_t *), (this))

////////////////////////////////////////////////////////////////////////////////
// Slate Structures
////////////////////////////////////////////////////////////////////////////////

typedef SLATE_DEFINE_CONNECTION(void const * pfnSlot) slate_connection_t;
typedef SLATE_DEFINE_ARRAY(slate_connection_t *) slate_signal_t;
typedef SLATE_DEFINE_ARRAY(slate_widget_t *)     slate_array_widget_t;
typedef SLATE_DEFINE_ARRAY(slate_connection_t *) slate_array_connection_t;

typedef void (SLATEPTR *slate_widget_destroy_pfn)(slate_widget_t *);
typedef int (SLATEPTR *slate_widget_recalc_pfn)(slate_widget_t *, slate_region_t const *);
typedef int (SLATEPTR *slate_widget_paint_pfn)(slate_widget_t *, slate_region_t const *);

//------------------------------------------------------------------------------
struct slate_widget_config_t {
  slate_alloc_t const *                 pAllocator;
  size_t                                widgetSize;
  slate_widget_destroy_pfn              pfnDestroy;
  slate_widget_recalc_pfn               pfnRecalculate;
  slate_widget_paint_pfn                pfnPaint;
};

//------------------------------------------------------------------------------
struct slate_widget_t {
  slate_alloc_t const *                 pAllocator;
  slate_widget_t *                      pParent;
  slate_state_t                         internalState;
  slate_array_connection_t              connections;
  slate_policy_t                        sizePolicy;
  slate_bounds_t                        minimumBounds;  // Minimum allowed bounds.
  slate_bounds_t                        maximumBounds;  // Maximum allowed bounds.
  slate_bounds_t                        contentBounds;  // Calculated content bounds (not always limited by min/max).
  slate_region_t                        innerRegion;    // The total region that contains content.
  slate_region_t                        outerRegion;    // The total region that the widget paints to.
  slate_widget_destroy_pfn              pfnDestroy;
  slate_widget_recalc_pfn               pfnRecalculate;
  slate_widget_paint_pfn                pfnPaint;
};

//------------------------------------------------------------------------------
struct slate_widget_parent_t {
  slate_widget_t                        base;
  slate_array_widget_t                  children;
};

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int SLATECALL __slate_create_widget (
  slate_widget_config_t const *         pConfig,
  slate_widget_t **                     pWidget
);

//------------------------------------------------------------------------------
#define slate_create_widget(pConfig, pWidget)                                   \
  __slate_create_widget(                                                        \
    pConfig,                                                                    \
    (slate_widget_t **)(pWidget)                                                \
  )

//------------------------------------------------------------------------------
void SLATECALL __slate_destroy_widget (
  slate_widget_t *                      pWidget
);

//------------------------------------------------------------------------------
#define slate_destroy_widget(pWidget)                                           \
  __slate_destroy_widget(                                                       \
    (slate_widget_t *)(pWidget)                                                 \
  )

//------------------------------------------------------------------------------
static inline int SLATECALL __slate_widget_recalculate (
  slate_widget_t *                      pWidget,
  slate_region_t const *                pRegion
) {
  return pWidget->pfnRecalculate(pWidget, pRegion);
}

//------------------------------------------------------------------------------
#define slate_widget_recalculate(pWidget, pRegion)                              \
  __slate_widget_recalculate(                                                   \
    (slate_widget_t *)(pWidget),                                                \
    pRegion                                                                     \
  )

//------------------------------------------------------------------------------
static inline int SLATECALL __slate_widget_paint (
  slate_widget_t *                      pWidget,
  slate_region_t const *                pRegion
) {
  return pWidget->pfnPaint(pWidget, pRegion);
}

//------------------------------------------------------------------------------
#define slate_widget_paint(pWidget, pRegion)                                    \
  __slate_widget_paint(                                                         \
    (slate_widget_t *)(pWidget),                                                \
    pRegion                                                                     \
  )

//------------------------------------------------------------------------------
int SLATECALL __slate_widget_prepare_connection (
  slate_widget_t *                      pSource,
  slate_widget_t *                      pTarget,
  slate_signal_t *                      pSignal
);

//------------------------------------------------------------------------------
#define slate_widget_connect(pSource, signal, pTarget, slot)                    \
  (                                                                             \
    __slate_widget_prepare_connection(                                          \
      (slate_widget_t *)(pSource),                                              \
      (slate_widget_t *)(pTarget),                                              \
      (slate_signal_t *)&(pSource)->signals.signal                              \
    ) ||                                                                        \
    !((pSource)->signals.signal.pData[                                          \
      (pSource)->signals.signal.count - 1                                       \
    ]->pfnSlot = (SLATE_SLOT_NAME(slot))&slot)                                  \
  ) ? (                                                                         \
    (                                                                           \
      ((pSource)->signals.signal.count == SLATE_SIGNAL_MAX) ||                  \
      (((slate_widget_t *)(pTarget))->connections.count == SLATE_SIGNAL_MAX)    \
    ) ? ERANGE : ENOMEM                                                         \
  ) : 0

//------------------------------------------------------------------------------
#define slate_widget_mark_state(pWidget, state)                                 \
  ((((slate_widget_t *)(pWidget)))->internalState |= (state))

//------------------------------------------------------------------------------
#define slate_widget_unmark_state(pWidget, state)                               \
  ((((slate_widget_t *)(pWidget)))->internalState &= ~(state))

//------------------------------------------------------------------------------
#define slate_widget_check_state(pWidget, state)                                \
  ((((slate_widget_t *)(pWidget)))->internalState & (state))

//------------------------------------------------------------------------------
#define slate_widget_set_state(pWidget, state, boolean)                         \
  (                                                                             \
    (boolean) ?                                                                 \
      slate_widget_mark_state(pWidget, state) :                                 \
      slate_widget_unmark_state(pWidget, state)                                 \
  )

//------------------------------------------------------------------------------
#define slate_widget_toggle_state(pWidget, state)                               \
  slate_widget_set_state(                                                       \
    pWidget,                                                                    \
    state,                                                                      \
    !slate_widget_check_state(pWidget, state)                                   \
  )

//------------------------------------------------------------------------------
#define slate_widget_is_visible(pWidget)                                        \
  slate_widget_check_state(pWidget, SLATE_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define slate_widget_mark_visible(pWidget)                                      \
  slate_widget_mark_state(pWidget, SLATE_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define slate_widget_unmark_visible(pWidget)                                    \
  slate_widget_unmark_state(pWidget, SLATE_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define slate_widget_set_visible(pWidget, boolean)                              \
  slate_widget_set_state(pWidget, SLATE_STATE_VISIBLE_BIT, boolean)

//------------------------------------------------------------------------------
#define slate_widget_toggle_visible(pWidget)                                    \
  slate_widget_toggle_state(pWidget, SLATE_STATE_VISIBLE_BIT)

//------------------------------------------------------------------------------
#define slate_widget_is_dirty(pWidget)                                          \
  slate_widget_check_state(pWidget, SLATE_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
slate_state_t __slate_widget_mark_dirty (
  slate_widget_t *                      pWidget
);

//------------------------------------------------------------------------------
#define slate_widget_mark_dirty(pWidget)                                        \
  __slate_widget_mark_dirty((slate_widget_t *)pWidget)

//------------------------------------------------------------------------------
#define slate_widget_unmark_dirty(pWidget)                                      \
  slate_widget_unmark_state(pWidget, SLATE_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define slate_widget_set_dirty(pWidget, boolean)                                \
  slate_widget_set_state(pWidget, SLATE_STATE_DIRTY_BIT, boolean)

//------------------------------------------------------------------------------
#define slate_widget_toggle_dirty(pWidget)                                      \
  slate_widget_toggle_state(pWidget, SLATE_STATE_DIRTY_BIT)

//------------------------------------------------------------------------------
#define slate_widget_check(pWidget)                                             \
  ((pWidget) && slate_widget_is_visible(pWidget))

//------------------------------------------------------------------------------
#define slate_widget_emit(pWidget, signal, ...)                                 \
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

#endif // SLATE_WIDGET_H
