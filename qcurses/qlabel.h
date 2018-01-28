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
#ifndef   QLABEL_H
#define   QLABEL_H

#include "qcurses.h"
#include "qwidget.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Label Definition
////////////////////////////////////////////////////////////////////////////////

// TODO: margin, indent, wordwrap mode (qwrap_t?).
//------------------------------------------------------------------------------
QWIDGET_BEGIN(qlabel_t)
  QWIDGET_SIGNALS_BEGIN
    QSIGNAL(set_align, qalign_t);
    QSIGNAL(set_text, char const *, size_t n);
  QWIDGET_SIGNALS_END
QWIDGET_END

////////////////////////////////////////////////////////////////////////////////
// Label Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_label (
  qalloc_t const *                      pAllocator,
  qlabel_t **                           pLabel
);

//------------------------------------------------------------------------------
void QCURSESCALL qdestroy_label (
  qlabel_t *                            pLabel
);

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_align (
  qlabel_t *                            pLabel,
  qalign_t                              alignment
);

//------------------------------------------------------------------------------
qalign_t QCURSESCALL qlabel_get_align (
  qlabel_t *                            pLabel
);

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_text (
  qlabel_t *                            pLabel,
  char const *                          text
);

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_text_n (
  qlabel_t *                            pLabel,
  char const *                          text,
  size_t                                n
);

//------------------------------------------------------------------------------
#define qlabel_set_text_k(pLabel, text)                                         \
  qlabel_set_text_n(pLabel, text, sizeof(text) - 1)

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QLABEL_H
