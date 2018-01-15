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
#ifndef   QCURSES_MATH_H
#define   QCURSES_MATH_H

#include "fwdqcurses.h"
#include <stdlib.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Math Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct qcurses_coord_t {
  size_t                                row;
  size_t                                column;
};

//------------------------------------------------------------------------------
struct qcurses_bounds_t {
  size_t                                rows;
  size_t                                columns;
};

//------------------------------------------------------------------------------
struct qcurses_region_t {
  qcurses_coord_t                       coord;
  qcurses_bounds_t                      bounds;
};

////////////////////////////////////////////////////////////////////////////////
// Math Functions
////////////////////////////////////////////////////////////////////////////////

static inline qcurses_coord_t QCURSESCALL qcurses_coord (
  size_t                                x,
  size_t                                y
) {
  qcurses_coord_t coord;
  coord.row     = y;
  coord.column  = x;
  return coord;
}

static inline int QCURSESCALL qcurses_coord_equal (
  qcurses_coord_t const *               pLhs,
  qcurses_coord_t const *               pRhs
) {
  return (pLhs->column == pRhs->column && pLhs->row == pRhs->row);
}

static inline qcurses_bounds_t QCURSESCALL qcurses_bounds (
  size_t                                rows,
  size_t                                columns
) {
  qcurses_bounds_t bounds;
  bounds.rows     = rows;
  bounds.columns  = columns;
  return bounds;
}

static inline int QCURSESCALL qcurses_bounds_equal (
  qcurses_bounds_t const *              pLhs,
  qcurses_bounds_t const *              pRhs
) {
  return (pLhs->columns == pRhs->columns && pLhs->rows == pRhs->rows);
}

static inline int QCURSESCALL qcurses_bounds_contains (
  qcurses_bounds_t const *              pBounds,
  qcurses_coord_t const *               pCoord
) {
  return (
    pBounds->rows     > pCoord->row     &&
    pBounds->columns  > pCoord->column
  );
}

static inline qcurses_region_t QCURSESCALL qcurses_region (
  size_t                                x,
  size_t                                y,
  size_t                                rows,
  size_t                                columns
) {
  qcurses_region_t region;
  region.coord  = qcurses_coord(x, y);
  region.bounds = qcurses_bounds(rows, columns);
  return region;
}

static inline int QCURSESCALL qcurses_region_equal (
  qcurses_region_t const *              pLhs,
  qcurses_region_t const *              pRhs
) {
  return
    qcurses_coord_equal(&pLhs->coord, &pRhs->coord) &&
    qcurses_bounds_equal(&pLhs->bounds, &pRhs->bounds);
}

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // QCURSES_MATH_H
