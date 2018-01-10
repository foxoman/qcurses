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
#ifndef   QCURSES_LABEL_H
#define   QCURSES_LABEL_H

#include "qcurses.h"
#include "widget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Label Definition
////////////////////////////////////////////////////////////////////////////////

// TODO: margin, indent, wordwrap mode (qcurses_wrap_t?).
//------------------------------------------------------------------------------
QCURSES_WIDGET_BEGIN(qcurses_label_t, qcurses_widget_t)
  // Intentionally Empty
QCURSES_WIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Label Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int qcurses_create_label (
  qcurses_alloc_t const *               pAllocator,
  qcurses_label_t **                    pLabel
);

//------------------------------------------------------------------------------
void qcurses_destroy_label (
  qcurses_label_t *                     pLabel
);

//------------------------------------------------------------------------------
int qcurses_label_set_align (
  qcurses_label_t *                     pLabel,
  qcurses_align_t                       alignment
);

//------------------------------------------------------------------------------
qcurses_align_t qcurses_label_get_align (
  qcurses_label_t *                     pLabel
);

//------------------------------------------------------------------------------
int qcurses_label_set_text (
  qcurses_label_t *                     pLabel,
  char const *                          text
);

//------------------------------------------------------------------------------
int qcurses_label_set_text_n (
  qcurses_label_t *                     pLabel,
  char const *                          text,
  size_t                                n
);

//------------------------------------------------------------------------------
#define qcurses_label_set_text_k(pLabel, text)                                  \
  qcurses_label_set_text_n(pLabel, text, sizeof(text) - 1)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_LABEL_H
