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
 * Slate Terminal UI Source
 ******************************************************************************/

// TODO: LT3 integration is not great here, this should be re-thought a little.
//       For now, we should just utilize LT3 for what we need and hack it in.
#define LT3_EMBEDDED
#include "label.h"
#include <ncurses.h>
#include <lt3/alloc.h>
#include <lt3/string.h>

// TODO: Text shifts when contentRegion > innerRegion, when ideally it should not.
#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

//------------------------------------------------------------------------------
struct SLATE_PIMPL_NAME(slate_label_t) {
  lt3_alloc_host_t                      allocator;
  slate_align_t                         alignment;
  lt3_string_t                          contents;
  size_t                                maxLineLength;
  SLATE_DEFINE_ARRAY(size_t)            lines;
};

//------------------------------------------------------------------------------
static int slate_label_recalculate (
  slate_label_t *                       pLabel,
  slate_region_t const *                pRegion
) {

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (slate_region_equal(&W(pLabel)->outerRegion, pRegion)) {
    return 0;
  }

  // TODO: Handle calculations for word wrapping, which can dynamically change the content size.
  //       For now, just assume no dynamic content size changes based on print region.
  slate_widget_mark_state(pLabel, SLATE_STATE_DIRTY_BIT);
  W(pLabel)->contentBounds  = slate_bounds(P(pLabel)->lines.count, P(pLabel)->maxLineLength);
  W(pLabel)->outerRegion    = *pRegion;

  return 0;
}

// TODO: Gross, do something else for this... Maybe pass a painter around that has dynamic whitespace data?
//       Figure this out after figuring out what to do about the recalculate/paint workflow.
static char const __slate_whitespace[] = "                                                                                                                                                                                                                                                                                        ";

//------------------------------------------------------------------------------
static int slate_label_paint (
  slate_label_t *                       pLabel,
  slate_region_t const *                pRegion
) {
  uint32_t idx;
  size_t lineLength;
  size_t printedLength;
  size_t columnOffset;
  size_t stringOffset;
  size_t lineOffset;
  size_t rowOffset;
  size_t rowCount;
  char const * pString;

  // Calculate the inner content region (union of contentBounds and outerRegion)
  // Calculate the row/column offset based on content length and alignment.
  // Note that this is for the entire printable region, not for an individual line.
  {
    W(pLabel)->contentBounds  = slate_bounds(
      P(pLabel)->lines.count,
      P(pLabel)->maxLineLength
    );
    W(pLabel)->innerRegion.bounds = slate_bounds(
      SLATE_MIN(W(pLabel)->contentBounds.rows, pRegion->bounds.rows),
      SLATE_MIN(W(pLabel)->contentBounds.columns, pRegion->bounds.columns)
    );
    switch (P(pLabel)->alignment & SLATE_ALIGN_HORIZONTAL_MASK) {
      case SLATE_ALIGN_LEFT_BIT:
        W(pLabel)->innerRegion.coord.column = 0;
        break;

      case SLATE_ALIGN_CENTER_BIT:
        W(pLabel)->innerRegion.coord.column = (pRegion->bounds.columns - W(pLabel)->innerRegion.bounds.columns) / 2;
        break;

      case SLATE_ALIGN_RIGHT_BIT:
        W(pLabel)->innerRegion.coord.column = pRegion->bounds.columns - W(pLabel)->innerRegion.bounds.columns;
        break;

      default:
        return EFAULT;
    }
    switch (P(pLabel)->alignment & SLATE_ALIGN_VERTICAL_MASK) {
      case SLATE_ALIGN_TOP_BIT:
        W(pLabel)->innerRegion.coord.row = 0;
        break;

      case SLATE_ALIGN_MIDDLE_BIT:
        W(pLabel)->innerRegion.coord.row = (pRegion->bounds.rows - W(pLabel)->innerRegion.bounds.rows) / 2;
        break;

      case SLATE_ALIGN_BOTTOM_BIT:
        W(pLabel)->innerRegion.coord.row = pRegion->bounds.rows - W(pLabel)->innerRegion.bounds.rows;
        break;

      default:
        return EFAULT;
    }
  }

  // Clear the outer region.
  for (idx = 0; idx < pRegion->bounds.rows; ++idx) {
    (void)move(pRegion->coord.row + idx, pRegion->coord.column);
    (void)addnstr(__slate_whitespace, pRegion->bounds.columns);
  }

  // Calculate the number of lines that will be cut off due to a small outerRegion.
  // We should utilize the vertical alignment mask to select where to cut content from.
  // TODO: Actually implement this!
  switch (P(pLabel)->alignment & SLATE_ALIGN_VERTICAL_MASK) {
    case SLATE_ALIGN_TOP_BIT:
      rowOffset = 0;
      lineOffset = 0;
      rowCount = SLATE_MIN(W(pLabel)->innerRegion.bounds.rows, P(pLabel)->lines.count);
      break;

    case SLATE_ALIGN_MIDDLE_BIT:
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

    case SLATE_ALIGN_BOTTOM_BIT:
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
  // TODO: Check ncurses results?
  pString = lt3_string_cstr(&P(pLabel)->contents);
  for (idx = 0; idx < rowCount; ++idx) {

    // Grab the current line length (excluding the newline).
    // Using this, we will calculate the printedLength which is visible lineLength.
    lineLength = P(pLabel)->lines.pData[idx + lineOffset];
    printedLength = SLATE_MIN(lineLength, W(pLabel)->innerRegion.bounds.columns);

    // If the printedLength is smaller than the lineLength, we have to cut some content.
    // We should utilize the horizontal alignment mask to either cut or pad content.
    switch (P(pLabel)->alignment & SLATE_ALIGN_HORIZONTAL_MASK) {
      case SLATE_ALIGN_LEFT_BIT:
        columnOffset = 0;
        stringOffset = 0;
        break;

      case SLATE_ALIGN_CENTER_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength) / 2;
        }
        else {
          columnOffset = (W(pLabel)->innerRegion.bounds.columns - lineLength) / 2;
          stringOffset = 0;
        }
        break;

      case SLATE_ALIGN_RIGHT_BIT:
        if (printedLength < lineLength) {
          columnOffset = 0;
          stringOffset = (lineLength - printedLength);
        }
        else {
          columnOffset = (W(pLabel)->innerRegion.bounds.columns - lineLength);
          stringOffset = 0;
        }
        break;
    }

    // Move to the ideal innerRegion offset and print the string.
    // Offset the pString pointer by the full lineLength (+1 for newline) for next line.
    (void)move(W(pLabel)->innerRegion.coord.row + rowOffset + idx, W(pLabel)->innerRegion.coord.column + columnOffset);
    (void)addnstr(pString + stringOffset, printedLength);
    pString += lineLength + 1;
  }

  slate_widget_unmark_dirty(pLabel);
  return 0;
}

//------------------------------------------------------------------------------
int slate_create_label (
  slate_alloc_t const *                 pAllocator,
  slate_label_t **                      pLabel
) {
  int err;
  slate_widget_config_t widgetConfig;
  slate_label_t * label;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator = pAllocator;
  widgetConfig.widgetSize = sizeof(slate_label_t) + sizeof(struct SLATE_PIMPL_NAME(slate_label_t));
  widgetConfig.pfnDestroy = (slate_widget_destroy_pfn)&slate_destroy_label;
  widgetConfig.pfnRecalculate = (slate_widget_recalc_pfn)&slate_label_recalculate;
  widgetConfig.pfnPaint = (slate_widget_paint_pfn)&slate_label_paint;

  // Allocate the terminal UI application.
  err = slate_create_widget(
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
  P(label) = (struct SLATE_PIMPL_NAME(slate_label_t)*)&label[1];
  P(label)->alignment = SLATE_ALIGN_MIDDLE_BIT | SLATE_ALIGN_CENTER_BIT;
  lt3_alloc_host_init(&P(label)->allocator);
  lt3_string_init(&P(label)->contents);

  // Return the application to the caller.
  *pLabel = label;
  return 0;
}

//------------------------------------------------------------------------------
void slate_destroy_label (
  slate_label_t *                       pLabel
) {
  lt3_string_deinit(&P(pLabel)->allocator.instance, &P(pLabel)->contents);
  slate_destroy_widget(pLabel);
}

//------------------------------------------------------------------------------
int slate_label_set_align (
  slate_label_t *                       pLabel,
  slate_align_t                         alignment
) {
  if (P(pLabel)->alignment != alignment) {
    P(pLabel)->alignment = alignment;
    slate_widget_mark_dirty(pLabel);
  }
  return 0;
}

//------------------------------------------------------------------------------
slate_align_t slate_label_get_align (
  slate_label_t *                       pLabel
) {
  return P(pLabel)->alignment;
}

//------------------------------------------------------------------------------
int slate_label_set_text (
  slate_label_t *                       pLabel,
  char const *                          text
) {
  return slate_label_set_text_n(
    pLabel,
    text,
    strlen(text)
  );
}

//------------------------------------------------------------------------------
int slate_label_set_text_n (
  slate_label_t *                       pLabel,
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

  slate_array_clear(&P(pLabel)->lines);

  // Construct the string views into the raw data as relative offsets.
  // These are the places where a newline occurs so we can calculate alignment later.
  maxLineLength = 0;
  for (last = 0, idx = 0; idx < pString->length; ++idx) {
    if (pData[idx] == '\n') {
      lineLength = idx - last;
      if (lineLength > maxLineLength) {
        maxLineLength = lineLength;
      }
      err = slate_array_push(W(pLabel)->pAllocator, &P(pLabel)->lines, lineLength);
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
    err = slate_array_push(W(pLabel)->pAllocator, &P(pLabel)->lines, lineLength);
    if (err) {
      return err;
    }
  }

  // Assign the line statistics.
  P(pLabel)->maxLineLength = maxLineLength;

  // If we succeeded, we should mark the widget as dirty.
  slate_widget_mark_dirty(pLabel);
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
