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

#include "qpainter.h"
#include "detail/qpainter.inl"
#include <ncurses.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// Painter Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int qpainter_clearins (
  qpainter_t *                          pPainter,
  qregion_t const *                     pRegion
) {
  int err;
  size_t currRow;

  for (currRow = 0; currRow < pRegion->bounds.rows; ++currRow) {
    err = mvwinsnstr(
      pPainter->pWindow,
      (int)(pRegion->coord.row + currRow),
      (int)(pRegion->coord.column),
      pPainter->pClearBrush,
      (int)(pRegion->bounds.columns)
    );
    if (err == ERR) {
      return EFAULT;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
static int qpainter_clearadd (
  qpainter_t *                          pPainter,
  qregion_t const *                     pRegion
) {
  int err;
  size_t currRow;

  for (currRow = 0; currRow < pRegion->bounds.rows; ++currRow) {
    err = mvwaddnstr(
      pPainter->pWindow,
      (int)(pRegion->coord.row + currRow),
      (int)(pRegion->coord.column),
      pPainter->pClearBrush,
      (int)(pRegion->bounds.columns)
    );
    if (err == ERR) {
      return EFAULT;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_clear (
  qpainter_t *                          pPainter,
  qregion_t const *                     pRegion
) {

  // Select between clearins and clearadd (clear mechanism).
  // This is because NCurses can report failure if we scroll the screen with scroll-lock.
  if (pPainter->boundary.columns == pRegion->coord.column + pRegion->bounds.columns) {
    return qpainter_clearins(pPainter, pRegion);
  }
  else {
    return qpainter_clearadd(pPainter, pRegion);
  }
}

//------------------------------------------------------------------------------
static size_t QCURSESCALL qcountprintable (
  char const *                          pData,
  size_t                                n
) {
  // TODO: For now, we are going to just assume all printable characters.
  return n;
}

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_paint (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
) {
  size_t printableCharacters;

  // Calculate the number of printable characters (non-command).
  printableCharacters = qcountprintable(pData, n);

  // Either select to add or insert depending on how this affects the screen.
  // We can fail to addstr if it will update the cursor past the screen boundary.
  // As a mitigation, we instead select between add/insert depending on parameters.
  if (pPainter->boundary.columns == pOrigin->column + printableCharacters) {
    return qpainter_insstr(pPainter, pOrigin, pData, n);
  }
  else {
    return qpainter_addstr(pPainter, pOrigin, pData, n);
  }
}

//------------------------------------------------------------------------------
static int qpainter_determine_failure (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
) {
  n = qcountprintable(pData, n);
  if (
    pPainter->boundary.columns  <= pOrigin->column      ||
    pPainter->boundary.rows     <= pOrigin->row         ||
    pPainter->boundary.columns  <  n                    ||
    pPainter->boundary.columns  <  pOrigin->column + n
  ) {
    return ERANGE;
  }
  return EFAULT;
}

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_insstr (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
) {
  int err;

  // Ignore the range check at this point and insert the string.
  // If the call fails, we can then do the range calculation to determine failure.
  err = mvwinsnstr(
    pPainter->pWindow,
    (int)(pOrigin->row),
    (int)(pOrigin->column),
    pData,
    (int)(n)
  );
  if (err == ERR) {
    return qpainter_determine_failure(
      pPainter,
      pOrigin,
      pData,
      n
    );
  }

  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qpainter_addstr (
  qpainter_t *                          pPainter,
  qcoord_t const *                      pOrigin,
  char const *                          pData,
  size_t                                n
) {
  int err;

  // Ignore the range check at this point and add the string.
  // If the call fails, we can then do the range calculation to determine failure.
  err = mvwaddnstr(
    pPainter->pWindow,
    (int)(pOrigin->row),
    (int)(pOrigin->column),
    pData,
    (int)(n)
  );
  if (err == ERR) {
    return qpainter_determine_failure(
      pPainter,
      pOrigin,
      pData,
      n
    );
  }

  return 0;
}
