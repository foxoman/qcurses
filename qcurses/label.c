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

// TODO: LT3 integration is not great here, this should be re-thought a little.
//       For now, we should just utilize LT3 for what we need and hack it in.
#define LT3_EMBEDDED
#include "label.h"
#include "painter.h"
#include <lt3/alloc.h>
#include <lt3/string.h>

// TODO: Text shifts when contentRegion > innerRegion, when ideally it should not.
#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Label Implementations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
struct QCURSES_PIMPL_NAME(qcurses_label_t) {
  lt3_alloc_host_t                      allocator;
  qcurses_align_t                       alignment;
  lt3_string_t                          contents;
  size_t                                maxLineLength;
  QCURSES_DEFINE_ARRAY(size_t)          lines;
};

////////////////////////////////////////////////////////////////////////////////
// Label Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_RECALC(
  qcurses_label_recalculate,
  qcurses_label_t *                     pLabel,
  qcurses_region_t const *              pRegion
) {

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (qcurses_region_equal(&W(pLabel)->outerRegion, pRegion)) {
    return 0;
  }

  // TODO: Handle calculations for word wrapping, which can dynamically change the content size.
  //       For now, just assume no dynamic content size changes based on print region.
  qcurses_widget_mark_state(pLabel, QCURSES_STATE_DIRTY_BIT);
  W(pLabel)->contentBounds  = qcurses_bounds(P(pLabel)->lines.count, P(pLabel)->maxLineLength);
  W(pLabel)->outerRegion    = *pRegion;

  return 0;
}

//------------------------------------------------------------------------------
QCURSES_PAINTER(
  qcurses_label_paint,
  qcurses_label_t *                     pLabel,
  qcurses_painter_t *                   pPainter
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
  qcurses_coord_t printCoord;

  // Calculate the inner content region (union of contentBounds and outerRegion)
  // Calculate the row/column offset based on content length and alignment.
  // Note that this is for the entire printable region, not for an individual line.
  {
    W(pLabel)->contentBounds  = qcurses_bounds(
      P(pLabel)->lines.count,
      P(pLabel)->maxLineLength
    );
    W(pLabel)->innerRegion.bounds = qcurses_bounds(
      QCURSES_MIN(W(pLabel)->contentBounds.rows, W(pLabel)->outerRegion.bounds.rows),
      QCURSES_MIN(W(pLabel)->contentBounds.columns, W(pLabel)->outerRegion.bounds.columns)
    );
    switch (P(pLabel)->alignment & QCURSES_ALIGN_HORIZONTAL_MASK) {
      case QCURSES_ALIGN_LEFT_BIT:
        W(pLabel)->innerRegion.coord.column = 0;
        break;

      case QCURSES_ALIGN_CENTER_BIT:
        W(pLabel)->innerRegion.coord.column = (W(pLabel)->outerRegion.bounds.columns - W(pLabel)->innerRegion.bounds.columns) / 2;
        break;

      case QCURSES_ALIGN_RIGHT_BIT:
        W(pLabel)->innerRegion.coord.column = W(pLabel)->outerRegion.bounds.columns - W(pLabel)->innerRegion.bounds.columns;
        break;

      default:
        return EFAULT;
    }
    switch (P(pLabel)->alignment & QCURSES_ALIGN_VERTICAL_MASK) {
      case QCURSES_ALIGN_TOP_BIT:
        W(pLabel)->innerRegion.coord.row = 0;
        break;

      case QCURSES_ALIGN_MIDDLE_BIT:
        W(pLabel)->innerRegion.coord.row = (W(pLabel)->outerRegion.bounds.rows - W(pLabel)->innerRegion.bounds.rows) / 2;
        break;

      case QCURSES_ALIGN_BOTTOM_BIT:
        W(pLabel)->innerRegion.coord.row = W(pLabel)->outerRegion.bounds.rows - W(pLabel)->innerRegion.bounds.rows;
        break;

      default:
        return EFAULT;
    }
  }

  // Clear the outer region.
  err = qcurses_painter_clear(
    pPainter,
    &W(pLabel)->outerRegion
  );
  if (err) {
    return err;
  }

  // Calculate the number of lines that will be cut off due to a small outerRegion.
  // We should utilize the vertical alignment mask to select where to cut content from.
  switch (P(pLabel)->alignment & QCURSES_ALIGN_VERTICAL_MASK) {
    case QCURSES_ALIGN_TOP_BIT:
      rowOffset = 0;
      lineOffset = 0;
      rowCount = QCURSES_MIN(W(pLabel)->innerRegion.bounds.rows, P(pLabel)->lines.count);
      break;

    case QCURSES_ALIGN_MIDDLE_BIT:
      if (W(pLabel)->innerRegion.bounds.rows < P(pLabel)->lines.count) {
        rowOffset = 0;
        lineOffset = (P(pLabel)->lines.count - W(pLabel)->innerRegion.bounds.rows) / 2;
        rowCount = W(pLabel)->innerRegion.bounds.rows;
      }
      else {
        rowOffset = (W(pLabel)->innerRegion.bounds.rows - P(pLabel)->lines.count) / 2;
        lineOffset = 0;
        rowCount = P(pLabel)->lines.count;
      }
      break;

    case QCURSES_ALIGN_BOTTOM_BIT:
      if (W(pLabel)->innerRegion.bounds.rows < P(pLabel)->lines.count) {
        rowOffset = 0;
        lineOffset = P(pLabel)->lines.count - W(pLabel)->innerRegion.bounds.rows;
        rowCount = W(pLabel)->innerRegion.bounds.rows;
      }
      else {
        rowOffset = W(pLabel)->innerRegion.bounds.rows - P(pLabel)->lines.count;
        lineOffset = 0;
        rowCount = P(pLabel)->lines.count;
      }
      break;

    default:
      return ENOTSUP;
  }

  // Print each of the lines
  pString = lt3_string_cstr(&P(pLabel)->contents);
  for (idx = 0; idx < rowCount; ++idx) {

    // Grab the current line length (excluding the newline).
    // Using this, we will calculate the printedLength which is visible lineLength.
    lineLength = P(pLabel)->lines.pData[idx + lineOffset];
    printedLength = QCURSES_MIN(lineLength, W(pLabel)->innerRegion.bounds.columns);

    // If the printedLength is smaller than the lineLength, we have to cut some content.
    // We should utilize the horizontal alignment mask to either cut or pad content.
    switch (P(pLabel)->alignment & QCURSES_ALIGN_HORIZONTAL_MASK) {
      case QCURSES_ALIGN_LEFT_BIT:
        columnOffset = 0;
        stringOffset = 0;
        break;

      case QCURSES_ALIGN_CENTER_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength) / 2;
        }
        else {
          columnOffset = (W(pLabel)->innerRegion.bounds.columns - lineLength) / 2;
          stringOffset = 0;
        }
        break;

      case QCURSES_ALIGN_RIGHT_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength);
        }
        else {
          columnOffset = (W(pLabel)->innerRegion.bounds.columns - lineLength);
          stringOffset = 0;
        }
        break;

      default:
        return EFAULT;
    }


    // Move to the ideal innerRegion offset and print the string.
    // Offset the pString pointer by the full lineLength (+1 for newline) for next line.
    printCoord = qcurses_coord(
      W(pLabel)->innerRegion.coord.column + columnOffset,
      W(pLabel)->innerRegion.coord.row + rowOffset + idx
    );
    err = qcurses_painter_paint(
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

  qcurses_widget_unmark_dirty(pLabel);
  return 0;
}

//------------------------------------------------------------------------------
int qcurses_create_label (
  qcurses_alloc_t const *               pAllocator,
  qcurses_label_t **                    pLabel
) {
  int err;
  qcurses_widget_config_t widgetConfig;
  qcurses_label_t * label;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pAllocator;
  widgetConfig.publicSize     = sizeof(qcurses_label_t);
  widgetConfig.privateSize    = sizeof(QCURSES_PIMPL_STRUCT(qcurses_label_t));
  widgetConfig.pfnDestroy     = QCURSES_DESTROY_PTR(qcurses_destroy_label);
  widgetConfig.pfnRecalculate = QCURSES_RECALC_PTR(qcurses_label_recalculate);
  widgetConfig.pfnPaint       = QCURSES_PAINTER_PTR(qcurses_label_paint);

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &label
  );
  if (err) {
    return err;
  }

  // Grab the application private implementation pointer.
  // TODO: For now, don't implement an allocator pass-through layer.
  //       Eventually we have to think of a creative way to bypass this issue.
  //       As I assume that we will have multiple allocation layers.
  P(label)->alignment = QCURSES_ALIGN_MIDDLE_BIT | QCURSES_ALIGN_CENTER_BIT;
  lt3_alloc_host_init(&P(label)->allocator);
  lt3_string_init(&P(label)->contents);

  // Return the application to the caller.
  *pLabel = label;
  return 0;
}

//------------------------------------------------------------------------------
void qcurses_destroy_label (
  qcurses_label_t *                     pLabel
) {
  lt3_string_deinit(&P(pLabel)->allocator.instance, &P(pLabel)->contents);
  qcurses_destroy_widget(pLabel);
}

//------------------------------------------------------------------------------
int qcurses_label_set_align (
  qcurses_label_t *                     pLabel,
  qcurses_align_t                       alignment
) {
  if (P(pLabel)->alignment != alignment) {
    P(pLabel)->alignment = alignment;
    qcurses_widget_mark_dirty(pLabel);
  }
  return 0;
}

//------------------------------------------------------------------------------
qcurses_align_t qcurses_label_get_align (
  qcurses_label_t *                     pLabel
) {
  return P(pLabel)->alignment;
}

//------------------------------------------------------------------------------
int qcurses_label_set_text (
  qcurses_label_t *                     pLabel,
  char const *                          text
) {
  return qcurses_label_set_text_n(
    pLabel,
    text,
    strlen(text)
  );
}

//------------------------------------------------------------------------------
int qcurses_label_set_text_n (
  qcurses_label_t *                     pLabel,
  char const *                          text,
  size_t                                n
) {
  int err;
  size_t idx;
  size_t last;
  char * pData;
  size_t lineLength;
  size_t maxLineLength;
  lt3_string_t * pString;

  // We will be using the string a lot here, so grab a pointer.
  // Mostly this is just for code cleanliness - makes things simpler.
  pString = &P(pLabel)->contents;

  // Attempt to set the string inside the label.
  err = lt3_string_assign_cstr_n(
    &P(pLabel)->allocator.instance,
    pString,
    text,
    n
  );
  if (err) {
    return err;
  }

  // Now that we have a new label string, grab it for ease of use.
  // In this case, we want to avoid constantly grabbing the string pointer.
  // There is some cost to this, since the string can be either embedded or external.
  pData = lt3_string_cstr(pString);

  qcurses_array_clear(&P(pLabel)->lines);

  // Construct the string views into the raw data as relative offsets.
  // These are the places where a newline occurs so we can calculate alignment later.
  maxLineLength = 0;
  for (last = 0, idx = 0; idx < pString->length; ++idx) {
    if (pData[idx] == '\n') {
      lineLength = idx - last;
      if (lineLength > maxLineLength) {
        maxLineLength = lineLength;
      }
      err = qcurses_array_push(W(pLabel)->pAllocator, &P(pLabel)->lines, lineLength);
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
    err = qcurses_array_push(W(pLabel)->pAllocator, &P(pLabel)->lines, lineLength);
    if (err) {
      return err;
    }
  }

  // Assign the line statistics.
  P(pLabel)->maxLineLength = maxLineLength;

  // If we succeeded, we should mark the widget as dirty.
  qcurses_widget_mark_dirty(pLabel);
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
