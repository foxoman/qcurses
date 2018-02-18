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

#include "qlabel.h"
#include "qpainter.h"
#include "detail/lt3alloc.h"
#include <lt3/string/pstring.h>

// TODO: Text shifts when contentRegion > innerRegion, when ideally it should not.
#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Label Implementations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct QPIMPL_NAME(qlabel_t) {
  qlt3alloc_t                           allocator;
  qalign_t                              alignment;
  lt3_pstring_t                         contents;
  size_t                                maxLineLength;
  QDEFINE_ARRAY(size_t)                 lines;
};

////////////////////////////////////////////////////////////////////////////////
// Label Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QRECALC(
  qlabel_recalculate,
  qlabel_t *                            pLabel,
  qregion_t const *                     pRegion
) {

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (qregion_equal(&QW(pLabel)->outerRegion, pRegion)) {
    return 0;
  }

  // TODO: Handle calculations for word wrapping, which can dynamically change the content size.
  //       For now, just assume no dynamic content size changes based on print region.
  qwidget_mark_state(pLabel, QSTATE_DIRTY_BIT);
  QW(pLabel)->contentBounds  = qbounds(
    QMIN(QP(pLabel)->lines.count, pRegion->bounds.rows),
    QP(pLabel)->maxLineLength
  );
  QW(pLabel)->outerRegion    = *pRegion;

  return 0;
}

//------------------------------------------------------------------------------
QPAINTER(
  qlabel_paint,
  qlabel_t *                            pLabel,
  qpainter_t *                          pPainter
) {
  int err;
  uint32_t idx;
  size_t lineLength;
  size_t printedLength;
  size_t columnOffset;
  size_t stringOffset;
  size_t lineOffset;
  size_t rowOffset;
  size_t rowCount;
  char const * pString;
  qcoord_t printCoord;

  // Calculate the inner content region (union of contentBounds and outerRegion)
  // Calculate the row/column offset based on content length and alignment.
  // Note that this is for the entire printable region, not for an individual line.
  {
    QW(pLabel)->contentBounds  = qbounds(
      QP(pLabel)->lines.count,
      QP(pLabel)->maxLineLength
    );
    QW(pLabel)->innerRegion.coord = QW(pLabel)->outerRegion.coord;
    QW(pLabel)->innerRegion.bounds = qbounds(
      QMIN(QW(pLabel)->contentBounds.rows, QW(pLabel)->outerRegion.bounds.rows),
      QMIN(QW(pLabel)->contentBounds.columns, QW(pLabel)->outerRegion.bounds.columns)
    );
    switch (QP(pLabel)->alignment & QALIGN_HORIZONTAL_MASK) {
      case QALIGN_LEFT_BIT:
        QW(pLabel)->innerRegion.coord.column += 0;
        break;

      case QALIGN_CENTER_BIT:
        QW(pLabel)->innerRegion.coord.column += (QW(pLabel)->outerRegion.bounds.columns - QW(pLabel)->innerRegion.bounds.columns) / 2;
        break;

      case QALIGN_RIGHT_BIT:
        QW(pLabel)->innerRegion.coord.column += QW(pLabel)->outerRegion.bounds.columns - QW(pLabel)->innerRegion.bounds.columns;
        break;

      default:
        return EFAULT;
    }
    switch (QP(pLabel)->alignment & QALIGN_VERTICAL_MASK) {
      case QALIGN_TOP_BIT:
        QW(pLabel)->innerRegion.coord.row += 0;
        break;

      case QALIGN_MIDDLE_BIT:
        QW(pLabel)->innerRegion.coord.row += (QW(pLabel)->outerRegion.bounds.rows - QW(pLabel)->innerRegion.bounds.rows) / 2;
        break;

      case QALIGN_BOTTOM_BIT:
        QW(pLabel)->innerRegion.coord.row += QW(pLabel)->outerRegion.bounds.rows - QW(pLabel)->innerRegion.bounds.rows;
        break;

      default:
        return EFAULT;
    }
  }

  // Clear the outer region.
  err = qpainter_clear(
    pPainter,
    &QW(pLabel)->outerRegion
  );
  if (err) {
    return err;
  }

  // Calculate the number of lines that will be cut off due to a small outerRegion.
  // We should utilize the vertical alignment mask to select where to cut content from.
  switch (QP(pLabel)->alignment & QALIGN_VERTICAL_MASK) {
    case QALIGN_TOP_BIT:
      rowOffset = 0;
      lineOffset = 0;
      rowCount = QMIN(QW(pLabel)->innerRegion.bounds.rows, QP(pLabel)->lines.count);
      break;

    case QALIGN_MIDDLE_BIT:
      if (QW(pLabel)->innerRegion.bounds.rows < QP(pLabel)->lines.count) {
        rowOffset = 0;
        lineOffset = (QP(pLabel)->lines.count - QW(pLabel)->innerRegion.bounds.rows) / 2;
        rowCount = QW(pLabel)->innerRegion.bounds.rows;
      }
      else {
        rowOffset = (QW(pLabel)->innerRegion.bounds.rows - QP(pLabel)->lines.count) / 2;
        lineOffset = 0;
        rowCount = QP(pLabel)->lines.count;
      }
      break;

    case QALIGN_BOTTOM_BIT:
      if (QW(pLabel)->innerRegion.bounds.rows < QP(pLabel)->lines.count) {
        rowOffset = 0;
        lineOffset = QP(pLabel)->lines.count - QW(pLabel)->innerRegion.bounds.rows;
        rowCount = QW(pLabel)->innerRegion.bounds.rows;
      }
      else {
        rowOffset = QW(pLabel)->innerRegion.bounds.rows - QP(pLabel)->lines.count;
        lineOffset = 0;
        rowCount = QP(pLabel)->lines.count;
      }
      break;

    default:
      return ENOTSUP;
  }

  // Print each of the lines
  pString = QP(pLabel)->contents->data;
  for (idx = 0; idx < rowCount; ++idx) {

    // Grab the current line length (excluding the newline).
    // Using this, we will calculate the printedLength which is visible lineLength.
    lineLength = QP(pLabel)->lines.pData[idx + lineOffset];
    printedLength = QMIN(lineLength, QW(pLabel)->innerRegion.bounds.columns);

    // If the printedLength is smaller than the lineLength, we have to cut some content.
    // We should utilize the horizontal alignment mask to either cut or pad content.
    switch (QP(pLabel)->alignment & QALIGN_HORIZONTAL_MASK) {
      case QALIGN_LEFT_BIT:
        columnOffset = 0;
        stringOffset = 0;
        break;

      case QALIGN_CENTER_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength) / 2;
        }
        else {
          columnOffset = (QW(pLabel)->innerRegion.bounds.columns - lineLength) / 2;
          stringOffset = 0;
        }
        break;

      case QALIGN_RIGHT_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength);
        }
        else {
          columnOffset = (QW(pLabel)->innerRegion.bounds.columns - lineLength);
          stringOffset = 0;
        }
        break;

      default:
        return EFAULT;
    }

    // Move to the ideal innerRegion offset and print the string.
    // Offset the pString pointer by the full lineLength (+1 for newline) for next line.
    printCoord = qcoord(
      QW(pLabel)->innerRegion.coord.column + columnOffset,
      QW(pLabel)->innerRegion.coord.row + rowOffset + idx
    );
    err = qpainter_paint(
      pPainter,
      &printCoord,
      pString + stringOffset,
      printedLength
    );
    if (err) {
      return err;
    }

    pString += lineLength + 1;
  }

  qwidget_unmark_dirty(pLabel);
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_label (
  qalloc_t const *                      pAllocator,
  qlabel_t **                           pLabel
) {
  int err;
  qwidget_config_t widgetConfig;
  qlabel_t * label;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(qlabel_t);
  widgetConfig.privateSize    = sizeof(QPIMPL_STRUCT(qlabel_t));
  widgetConfig.pfnDestroy     = QDESTROY_PTR(qdestroy_label);
  widgetConfig.pfnRecalculate = QRECALC_PTR(qlabel_recalculate);
  widgetConfig.pfnPaint       = QPAINTER_PTR(qlabel_paint);

  // Allocate the terminal UI application.
  err = qcreate_widget(
    &widgetConfig,
    &label
  );
  if (err) {
    return err;
  }

  // Grab the application private implementation pointer.
  QP(label)->alignment = QALIGN_MIDDLE_BIT | QALIGN_CENTER_BIT;
  qlt3alloc_init(QW(label)->pAllocator, &QP(label)->allocator);

  // Return the application to the caller.
  *pLabel = label;
  return 0;
}

//------------------------------------------------------------------------------
void QCURSESCALL qdestroy_label (
  qlabel_t *                            pLabel
) {
  lt3_pstring_deinit(&QP(pLabel)->allocator.instance, QP(pLabel)->contents);
  qdestroy_widget(pLabel);
}

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_align (
  qlabel_t *                            pLabel,
  qalign_t                              alignment
) {
  if (QP(pLabel)->alignment != alignment) {
    QP(pLabel)->alignment = alignment;
    qwidget_mark_dirty(pLabel);
  }
  qwidget_emit(pLabel, set_align, alignment);
  return 0;
}

//------------------------------------------------------------------------------
qalign_t QCURSESCALL qlabel_get_align (
  qlabel_t *                            pLabel
) {
  return QP(pLabel)->alignment;
}

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_text (
  qlabel_t *                            pLabel,
  char const *                          text
) {
  return qlabel_set_text_n(
    pLabel,
    text,
    strlen(text)
  );
}

//------------------------------------------------------------------------------
int QCURSESCALL qlabel_set_text_n (
  qlabel_t *                            pLabel,
  char const *                          text,
  size_t                                n
) {
  int err;
  size_t idx;
  size_t last;
  char * pData;
  size_t lineLength;
  size_t maxLineLength;
  lt3_pstring_t pString;

  // We will be using the string a lot here, so grab a pointer.
  // Mostly this is just for code cleanliness - makes things simpler.
  pString = QP(pLabel)->contents;

  // Attempt to set the string inside the label.
  err = lt3_pstring_assign_cstr_n(
    &QP(pLabel)->allocator.instance,
    &pString,
    text,
    n
  );
  if (err) {
    return err;
  }

  // Now that we have a new label string, grab it for ease of use.
  // In this case, we want to avoid constantly grabbing the string pointer.
  // There is some cost to this, since the string can be either embedded or external.
  pData = pString->data;
  QP(pLabel)->contents = pString;
  qarray_clear(&QP(pLabel)->lines);

  // Construct the string views into the raw data as relative offsets.
  // These are the places where a newline occurs so we can calculate alignment later.
  maxLineLength = 0;
  for (last = 0, idx = 0; idx < pString->length; ++idx) {
    if (pData[idx] == '\n') {
      lineLength = idx - last;
      if (lineLength > maxLineLength) {
        maxLineLength = lineLength;
      }
      err = qarray_push(QW(pLabel)->pAllocator, &QP(pLabel)->lines, lineLength);
      if (err) {
        return err;
      }
      last = idx + 1;
    }
  }
  if (idx != last) {
    lineLength = idx - last;
    if (lineLength > maxLineLength) {
      maxLineLength = lineLength;
    }
    err = qarray_push(QW(pLabel)->pAllocator, &QP(pLabel)->lines, lineLength);
    if (err) {
      return err;
    }
  }

  // Assign the line statistics.
  QP(pLabel)->maxLineLength = maxLineLength;

  // If we succeeded, we should mark the widget as dirty.
  qwidget_mark_dirty(pLabel);
  qwidget_emit(pLabel, set_text, pData, n);
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
