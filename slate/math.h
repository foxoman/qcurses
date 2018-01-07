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
#ifndef   SLATE_MATH_H
#define   SLATE_MATH_H

#include "fwdslate.h"

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Structures
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct slate_coord_t {
  size_t                                row;
  size_t                                column;
};

//------------------------------------------------------------------------------
struct slate_bounds_t {
  size_t                                rows;
  size_t                                columns;
};

//------------------------------------------------------------------------------
struct slate_region_t {
  slate_coord_t                         coord;
  slate_bounds_t                        bounds;
};

////////////////////////////////////////////////////////////////////////////////
// Slate API
////////////////////////////////////////////////////////////////////////////////

static inline slate_coord_t SLATECALL slate_coord(size_t x, size_t y) {
  slate_coord_t coord;
  coord.row     = y;
  coord.column  = x;
  return coord;
}

static inline int SLATECALL slate_coord_equal(slate_coord_t const * pLhs, slate_coord_t const * pRhs) {
  return (pLhs->column == pRhs->column && pLhs->row == pRhs->row);
}

static inline slate_bounds_t SLATECALL slate_bounds(size_t rows, size_t columns) {
  slate_bounds_t bounds;
  bounds.rows     = rows;
  bounds.columns  = columns;
  return bounds;
}

static inline int SLATECALL slate_bounds_equal(slate_bounds_t const * pLhs, slate_bounds_t const * pRhs) {
  return (pLhs->columns == pRhs->columns && pLhs->rows == pRhs->rows);
}

static inline slate_region_t SLATECALL slate_region(size_t x, size_t y, size_t rows, size_t columns) {
  slate_region_t region;
  region.coord  = slate_coord(x, y);
  region.bounds = slate_bounds(rows, columns);
  return region;
}

static inline int SLATECALL slate_region_equal(slate_region_t const * pLhs, slate_region_t const * pRhs) {
  return
    slate_coord_equal(&pLhs->coord, &pRhs->coord) &&
    slate_bounds_equal(&pLhs->bounds, &pRhs->bounds);
}

#ifdef    __cplusplus
}
#endif // __cplusplus

#endif // SLATE_MATH_H
