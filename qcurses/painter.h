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
#ifndef   QCURSES_PAINTER_H
#define   QCURSES_PAINTER_H

#include "fwdqcurses.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Painter Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
int qcurses_painter_clear (
  qcurses_painter_t *                   pPainter,
  qcurses_region_t const *              pRegion
);

//------------------------------------------------------------------------------
int qcurses_painter_paint (
  qcurses_painter_t *                   pPainter,
  qcurses_coord_t const *               pOrigin,
  char const *                          pData,
  size_t                                n
);

//------------------------------------------------------------------------------
int qcurses_painter_insstr (
  qcurses_painter_t *                   pPainter,
  qcurses_coord_t const *               pOrigin,
  char const *                          pData,
  size_t                                n
);

//------------------------------------------------------------------------------
int qcurses_painter_addstr (
  qcurses_painter_t *                   pPainter,
  qcurses_coord_t const *               pOrigin,
  char const *                          pData,
  size_t                                n
);

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_PAINTER_H
