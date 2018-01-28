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
#ifndef   QMATH_H
#define   QMATH_H

#include "qcurses.h"
#include <stdlib.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Math Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct qcoord_t {
  size_t                                row;
  size_t                                column;
};

//------------------------------------------------------------------------------
struct qbounds_t {
  size_t                                rows;
  size_t                                columns;
};

//------------------------------------------------------------------------------
struct qregion_t {
  qcoord_t                              coord;
  qbounds_t                             bounds;
};

////////////////////////////////////////////////////////////////////////////////
// Math Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static inline qcoord_t QCURSESCALL qcoord (
  size_t                                x,
  size_t                                y
) {
  qcoord_t coord;
  coord.row     = y;
  coord.column  = x;
  return coord;
}

//------------------------------------------------------------------------------
static inline int QCURSESCALL qcoord_equal (
  qcoord_t const *                      pLhs,
  qcoord_t const *                      pRhs
) {
  return (pLhs->column == pRhs->column && pLhs->row == pRhs->row);
}

//------------------------------------------------------------------------------
static inline qbounds_t QCURSESCALL qbounds (
  size_t                                rows,
  size_t                                columns
) {
  qbounds_t bounds;
  bounds.rows     = rows;
  bounds.columns  = columns;
  return bounds;
}

//------------------------------------------------------------------------------
static inline int QCURSESCALL qbounds_equal (
  qbounds_t const *                     pLhs,
  qbounds_t const *                     pRhs
) {
  return (pLhs->columns == pRhs->columns && pLhs->rows == pRhs->rows);
}

//------------------------------------------------------------------------------
static inline int QCURSESCALL qbounds_contains (
  qbounds_t const *              pBounds,
  qcoord_t const *               pCoord
) {
  return (
    pBounds->rows     > pCoord->row     &&
    pBounds->columns  > pCoord->column
  );
}

//------------------------------------------------------------------------------
static inline qregion_t QCURSESCALL qregion (
  size_t                                x,
  size_t                                y,
  size_t                                rows,
  size_t                                columns
) {
  qregion_t region;
  region.coord  = qcoord(x, y);
  region.bounds = qbounds(rows, columns);
  return region;
}

//------------------------------------------------------------------------------
static inline int QCURSESCALL qregion_equal (
  qregion_t const *                     pLhs,
  qregion_t const *                     pRhs
) {
  return
    qcoord_equal(&pLhs->coord, &pRhs->coord) &&
    qbounds_equal(&pLhs->bounds, &pRhs->bounds);
}

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QMATH_H
