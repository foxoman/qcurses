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
#ifndef   QPAINTER_H
#define   QPAINTER_H

#include "qcurses.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Painter Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_clear (
  qpainter_t *                          pPainter,
  qregion_t const *                     pRegion
);

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_paint (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
);

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_insstr (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
);

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_addstr (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QPAINTER_H
