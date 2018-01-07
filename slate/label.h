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
#ifndef   SLATE_LABEL_H
#define   SLATE_LABEL_H

#include "slate.h"
#include "widget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Definition
////////////////////////////////////////////////////////////////////////////////

// TODO: margin, indent, wordwrap mode (slate_wrap_t?).
//------------------------------------------------------------------------------
SLATE_WIDGET_BEGIN(slate_label_t, slate_widget_t)
  // Intentionally Empty
SLATE_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int slate_create_label (
  slate_alloc_t const *                 pAllocator,
  slate_label_t **                      pLabel
);

//------------------------------------------------------------------------------
void slate_destroy_label (
  slate_label_t *                       pLabel
);

//------------------------------------------------------------------------------
int slate_label_set_align (
  slate_label_t *                       pLabel,
  slate_align_t                         alignment
);

//------------------------------------------------------------------------------
slate_align_t slate_label_get_align (
  slate_label_t *                       pLabel
);

//------------------------------------------------------------------------------
int slate_label_set_text (
  slate_label_t *                       pLabel,
  char const *                          text
);

//------------------------------------------------------------------------------
int slate_label_set_text_n (
  slate_label_t *                       pLabel,
  char const *                          text,
  size_t                                n
);

//------------------------------------------------------------------------------
#define slate_label_set_text_k(pLabel, text)                                    \
  slate_label_set_text_n(pLabel, text, sizeof(text) - 1)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_LABEL_H
