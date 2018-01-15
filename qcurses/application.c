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

#include "application.h"
#include "painter.inl"
#include <ncurses.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Application Implementations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_PIMPL_STRUCT(qcurses_application_t) {
  qcurses_bool_t                        isQuitting;
  qcurses_bool_t                        isResizing;
  qcurses_bool_t                        hasMouse;
  qcurses_bool_t                        hasColors;
  qcurses_bool_t                        canChangeColors;
  qcurses_coord_t                       mouseCoord;
  mmask_t                               mouseEvents;
  qcurses_mouse_t                       prevMouseState;
  qcurses_painter_t                     painter;
  qcurses_widget_t *                    pMainWidget;
  qcurses_menu_bar_t *                  pMenuBar;       // TODO: Implement.
  qcurses_status_bar_t *                pStatusBar;     // TODO: Implement.
};

////////////////////////////////////////////////////////////////////////////////
// Application Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define QCURSES_CASE(theirs, ours) case theirs: code = ours; break
static int qcurses_application_update_input (
  qcurses_application_t *               pApplication
) {
  int value;
  MEVENT mouseEvent;
  qcurses_keycode_t code;

  // TODO: Add the shift/Ctrl/Alt key modifiers - this could be useful to some.
  //       https://stackoverflow.com/questions/9750588/how-to-get-ctrl-shift-or-alt-with-getch-ncurses
  value = wgetch(P(pApplication)->painter.pWindow);
  switch (value) {

    // Ignored input events
    case ERR:
    case KEY_EVENT:
      return ENODATA;
    case KEY_MOUSE:
      if (getmouse(&mouseEvent) == OK) {
        qcurses_mouse_t mouseState = 0;
        if (mouseEvent.bstate & BUTTON1_PRESSED) {
          mouseState |= QCURSES_MOUSE_BUTTON1_PRESSED_BIT;
          P(pApplication)->prevMouseState |= QCURSES_MOUSE_BUTTON1_PRESSED_BIT;
        }
        if (mouseEvent.bstate & BUTTON1_RELEASED) {
          mouseState |= QCURSES_MOUSE_BUTTON1_RELEASED_BIT;
          P(pApplication)->prevMouseState &= ~QCURSES_MOUSE_BUTTON1_PRESSED_BIT;
        }
        if (mouseEvent.bstate & BUTTON2_PRESSED) {
          mouseState |= QCURSES_MOUSE_BUTTON2_PRESSED_BIT;
          P(pApplication)->prevMouseState |= QCURSES_MOUSE_BUTTON2_PRESSED_BIT;
        }
        if (mouseEvent.bstate & BUTTON2_RELEASED) {
          mouseState |= QCURSES_MOUSE_BUTTON2_RELEASED_BIT;
          P(pApplication)->prevMouseState &= ~QCURSES_MOUSE_BUTTON2_PRESSED_BIT;
        }
        // TODO: Handle z (mousewheel).
        P(pApplication)->mouseCoord = qcurses_coord(
          (size_t)mouseEvent.x,
          (size_t)mouseEvent.y
        );
        mouseState |= P(pApplication)->prevMouseState;
        qcurses_widget_emit(pApplication, onMouse, &P(pApplication)->mouseCoord, mouseState);
      }
      return ENODATA;
    case KEY_RESIZE:
      P(pApplication)->isResizing = QCURSES_TRUE;
      return ENODATA;

    // Standard ASCII range
    case 'a' ... 'z':
    case '0' ... '9':
      code = (qcurses_keycode_t)value;
      break;
    case 'A' ... 'Z':
      code = (qcurses_keycode_t)tolower(value);
      break;

    // Standard ASCII range (mapped)
    QCURSES_CASE('\x1B', QCURSES_KEYCODE_ESCAPE);
    QCURSES_CASE('`', QCURSES_KEYCODE_BACKTICK);
    QCURSES_CASE('~', QCURSES_KEYCODE_BACKTICK);
    QCURSES_CASE('!', QCURSES_KEYCODE_1);
    QCURSES_CASE('@', QCURSES_KEYCODE_2);
    QCURSES_CASE('#', QCURSES_KEYCODE_3);
    QCURSES_CASE('$', QCURSES_KEYCODE_4);
    QCURSES_CASE('%', QCURSES_KEYCODE_5);
    QCURSES_CASE('^', QCURSES_KEYCODE_6);
    QCURSES_CASE('&', QCURSES_KEYCODE_7);
    QCURSES_CASE('*', QCURSES_KEYCODE_8);
    QCURSES_CASE('(', QCURSES_KEYCODE_9);
    QCURSES_CASE(')', QCURSES_KEYCODE_0);
    QCURSES_CASE('-', QCURSES_KEYCODE_MINUS);
    QCURSES_CASE('_', QCURSES_KEYCODE_MINUS);
    QCURSES_CASE('=', QCURSES_KEYCODE_EQUALS);
    QCURSES_CASE('+', QCURSES_KEYCODE_EQUALS);
    QCURSES_CASE('\t', QCURSES_KEYCODE_TAB);
    QCURSES_CASE('[', QCURSES_KEYCODE_LEFT_BRACKET);
    QCURSES_CASE('{', QCURSES_KEYCODE_LEFT_BRACKET);
    QCURSES_CASE(']', QCURSES_KEYCODE_RIGHT_BRACKET);
    QCURSES_CASE('}', QCURSES_KEYCODE_RIGHT_BRACKET);
    QCURSES_CASE('\\', QCURSES_KEYCODE_REVERSE_SOLIDUS);
    QCURSES_CASE('|', QCURSES_KEYCODE_REVERSE_SOLIDUS);
    QCURSES_CASE(';', QCURSES_KEYCODE_SEMICOLON);
    QCURSES_CASE(':', QCURSES_KEYCODE_SEMICOLON);
    QCURSES_CASE('\'', QCURSES_KEYCODE_SINGLE_QUOTE);
    QCURSES_CASE('"', QCURSES_KEYCODE_SINGLE_QUOTE);
    QCURSES_CASE('\n', QCURSES_KEYCODE_RETURN);
    QCURSES_CASE(',', QCURSES_KEYCODE_COMMA);
    QCURSES_CASE('<', QCURSES_KEYCODE_COMMA);
    QCURSES_CASE('.', QCURSES_KEYCODE_PERIOD);
    QCURSES_CASE('>', QCURSES_KEYCODE_PERIOD);
    QCURSES_CASE('/', QCURSES_KEYCODE_SOLIDUS);
    QCURSES_CASE('?', QCURSES_KEYCODE_SOLIDUS);
    QCURSES_CASE(' ', QCURSES_KEYCODE_SPACE);

    // Special ncurses keycodes
    QCURSES_CASE(KEY_DOWN, QCURSES_KEYCODE_DOWN);
    QCURSES_CASE(KEY_UP, QCURSES_KEYCODE_UP);
    QCURSES_CASE(KEY_LEFT, QCURSES_KEYCODE_LEFT);
    QCURSES_CASE(KEY_RIGHT, QCURSES_KEYCODE_RIGHT);
    QCURSES_CASE(KEY_HOME, QCURSES_KEYCODE_HOME);
    QCURSES_CASE(KEY_BACKSPACE, QCURSES_KEYCODE_BACKSPACE);
    QCURSES_CASE(KEY_F(0), QCURSES_KEYCODE_F0);
    QCURSES_CASE(KEY_F(1), QCURSES_KEYCODE_F1);
    QCURSES_CASE(KEY_F(2), QCURSES_KEYCODE_F2);
    QCURSES_CASE(KEY_F(3), QCURSES_KEYCODE_F3);
    QCURSES_CASE(KEY_F(4), QCURSES_KEYCODE_F4);
    QCURSES_CASE(KEY_F(5), QCURSES_KEYCODE_F5);
    QCURSES_CASE(KEY_F(6), QCURSES_KEYCODE_F6);
    QCURSES_CASE(KEY_F(7), QCURSES_KEYCODE_F7);
    QCURSES_CASE(KEY_F(8), QCURSES_KEYCODE_F8);
    QCURSES_CASE(KEY_F(9), QCURSES_KEYCODE_F9);
    QCURSES_CASE(KEY_F(10), QCURSES_KEYCODE_F10);
    QCURSES_CASE(KEY_F(11), QCURSES_KEYCODE_F11);
    QCURSES_CASE(KEY_F(12), QCURSES_KEYCODE_F12);
    QCURSES_CASE(KEY_DC, QCURSES_KEYCODE_DELETE);
    QCURSES_CASE(KEY_IC, QCURSES_KEYCODE_INSERT);
    QCURSES_CASE(KEY_SF, QCURSES_KEYCODE_DOWN);
    QCURSES_CASE(KEY_SR, QCURSES_KEYCODE_UP);
    QCURSES_CASE(KEY_NPAGE, QCURSES_KEYCODE_PAGE_DOWN);
    QCURSES_CASE(KEY_PPAGE, QCURSES_KEYCODE_PAGE_UP);
    QCURSES_CASE(KEY_BTAB, QCURSES_KEYCODE_TAB);
    QCURSES_CASE(KEY_ENTER, QCURSES_KEYCODE_RETURN);
    QCURSES_CASE(KEY_END, QCURSES_KEYCODE_END);
    QCURSES_CASE(KEY_SDC, QCURSES_KEYCODE_DELETE);
    QCURSES_CASE(KEY_SEND, QCURSES_KEYCODE_END);
    QCURSES_CASE(KEY_SHOME, QCURSES_KEYCODE_HOME);
    QCURSES_CASE(KEY_SLEFT, QCURSES_KEYCODE_LEFT);
    QCURSES_CASE(KEY_SNEXT, QCURSES_KEYCODE_PAGE_DOWN);
    QCURSES_CASE(KEY_SPREVIOUS, QCURSES_KEYCODE_PAGE_UP);
    QCURSES_CASE(KEY_SRIGHT, QCURSES_KEYCODE_RIGHT);

    // Unhandled (error)
    default:
      code = QCURSES_KEYCODE_INVALID;
      mvprintw(0, 0, "Unhandled value: 0x%x (%d) (0%o)\n", value, value, value);
      break;
  }
  qcurses_widget_emit(pApplication, onKey, code, value);

  return 0;
}
#undef QCURSES_CASE

//------------------------------------------------------------------------------
QCURSES_RECALC(
  qcurses_application_recalculate,
  qcurses_application_t *               pThis,
  qcurses_region_t const *              pRegion
) {
  int err;
  char * pNewBuffer;

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (qcurses_region_equal(&W(pThis)->outerRegion, pRegion)) {
    return 0;
  }

  // Update the application's painter instance.
  P(pThis)->painter.boundary = pRegion->bounds;
  if (P(pThis)->painter.maxBounds.columns < pRegion->bounds.columns) {
    pNewBuffer = qcurses_reallocate(
      W(pThis)->pAllocator,
      P(pThis)->painter.pClearBrush,
      pRegion->bounds.columns
    );
    if (!pNewBuffer) {
      return ENOMEM;
    }
    memset(pNewBuffer, ' ', pRegion->bounds.columns);
    P(pThis)->painter.pClearBrush = pNewBuffer;
    P(pThis)->painter.maxBounds.columns = pRegion->bounds.columns;
  }
  if (P(pThis)->painter.maxBounds.rows < pRegion->bounds.rows) {
    P(pThis)->painter.maxBounds.rows = pRegion->bounds.rows;
  }

  // Unlike other recalculate functions, application is special.
  // Application will ignore minimum/maximum size since it has no control over it.
  // Application also ignores size policies, instead always fixing to the size of the screen.
  // Because of this, in case the user erroneously changed these values, we should reset.
  qcurses_widget_mark_state(pThis, QCURSES_STATE_DIRTY_BIT);
  W(pThis)->sizePolicy     = QCURSES_POLICY_FIXED;
  W(pThis)->minimumBounds  = qcurses_bounds(0, 0);
  W(pThis)->maximumBounds  = qcurses_bounds(QCURSES_INFINITE, QCURSES_INFINITE);
  W(pThis)->contentBounds  = pRegion->bounds;
  W(pThis)->outerRegion    = *pRegion;

  // The central widget should fill the remaining space that menu/status aren't filling.
  // TODO: Right now, menu_bar and status_bar aren't implemented - so same as pRegion.
  if (qcurses_widget_check(P(pThis)->pMainWidget)) {
    err = qcurses_widget_recalculate(P(pThis)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
QCURSES_PAINTER(
  qcurses_application_paint,
  qcurses_application_t *               pThis,
  qcurses_painter_t *                   pPainter
) {
  int err;

  // Paint the central widget with the remaining region of the terminal.
  if (qcurses_widget_check(P(pThis)->pMainWidget)) {
    err = qcurses_widget_paint(P(pThis)->pMainWidget, pPainter);
    if (err) {
      return err;
    }
  }

  qcurses_widget_unmark_dirty(pThis);
  return 0;
}

//------------------------------------------------------------------------------
static int qcurses_application_update_resize (
  qcurses_application_t *               pApplication
) {
  int err;
  int x, y;
  qcurses_region_t resize;

  // Grab the current max x/y.
  err = getmaxyx(stdscr, y, x);
  if (err == ERR || x < 0 || y < 0) {
    return EFAULT;
  }

  // We have valid values, let's cast to the correct integral types.
  resize.coord.column   = 0;
  resize.coord.row      = 0;
  resize.bounds.columns = (size_t)x;
  resize.bounds.rows    = (size_t)y;

  // If they haven't changed from our previous x/y, nothing to do.
  qcurses_bool_t const screenSizeChanged = QCURSES_BOOL(
    (W(pApplication)->contentBounds.columns != resize.bounds.columns) ||
    (W(pApplication)->contentBounds.rows    != resize.bounds.rows   )
  );
  if (!screenSizeChanged) {
    return 0;
  }

  // Update the last-known application screen sizes.
  qcurses_widget_emit(pApplication, onResize, &resize.bounds);

  // Recalculate the sizes of all the widgets within the application.
  err = qcurses_application_recalculate(pApplication, &resize);
  if (err) {
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
static int qcurses_application_update (
  qcurses_application_t *               pApplication
) {
  int err;

  // Process all pending input (if available).
  err = nodelay(P(pApplication)->painter.pWindow, TRUE);
  if (err == ERR) {
    return EFAULT;
  }
  do {
    err = qcurses_application_update_input(pApplication);
  } while (!err);
  if (err != ENODATA) {
    return err;
  }

  // Handle a state-change to quit.
  if (P(pApplication)->isQuitting) {
    return 0;
  }

  // Process pending resize (if resized).
  if (P(pApplication)->isResizing) {
    P(pApplication)->isResizing = QCURSES_FALSE;
    err = qcurses_application_update_resize(pApplication);
    if (err) {
      return err;
    }
  }

  // Handle the visual update iff the application is marked dirty.
  if (qcurses_widget_is_dirty(pApplication)) {
    err = qcurses_application_paint(pApplication, &P(pApplication)->painter);
    if (err) {
      return err;
    }
    refresh();
  }

  // Handle a state-change to quit.
  if (P(pApplication)->isQuitting) {
    return 0;
  }

  // Block for any kind of update which might cause output to change.
  // No big deal if we don't process all input - head logic will capture the rest.
  err = nodelay(P(pApplication)->painter.pWindow, FALSE);
  if (err == ERR) {
    return EFAULT;
  }
  err = qcurses_application_update_input(pApplication);
  if (err == ERR) {
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
int qcurses_create_application (
  qcurses_application_info_t const *    pCreateInfo,
  qcurses_application_t **              pApplication
) {
  int err;
  qcurses_widget_config_t widgetConfig;
  qcurses_application_t * application;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator = pCreateInfo->pAllocator;
  widgetConfig.publicSize = sizeof(qcurses_application_t);
  widgetConfig.privateSize = sizeof(QCURSES_PIMPL_STRUCT(qcurses_application_t));
  widgetConfig.pfnDestroy = (qcurses_widget_destroy_pfn)&qcurses_destroy_application;
  widgetConfig.pfnRecalculate = QCURSES_RECALC_PTR(qcurses_application_recalculate);
  widgetConfig.pfnPaint = QCURSES_PAINTER_PTR(qcurses_application_paint);

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &application
  );
  if (err) {
    return err;
  }

  // Return the application to the caller.
  *pApplication = application;
  return 0;
}

//------------------------------------------------------------------------------
void qcurses_destroy_application (
  qcurses_application_t *               pApplication
) {
  // TODO: Destroy application properly.
  (void)pApplication;
}

//------------------------------------------------------------------------------
static int qcurses_application_start (
  qcurses_application_t *               pApplication
) {
  int err;

  // Construct the main window
  P(pApplication)->painter.pWindow = initscr();
  if (!P(pApplication)->painter.pWindow) {
    return EFAULT;
  }

  // Attempt to enable the special keys (arrows, function, etc.)
  err = keypad(stdscr, TRUE);
  if (err == ERR) {
    return EFAULT;
  }

  // Attempt to configure the canvas to be used (has_mouse must come after mousemask).
  // mouseinterval is used to force the default button state change to happen instantly.
  // Some terminals need fed commands to know that they should send canvas position events.
  // Note: See console_codes (4) for more information.
  P(pApplication)->mouseEvents = mousemask(
    ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION,
    NULL
  );
  err = mouseinterval(0);
  if (err == ERR) {
    return EFAULT;
  }
  err = fputs("\033[?1003h\n", stdout);
  if (err == EOF) {
    return EFAULT;
  }
  P(pApplication)->hasMouse = has_mouse();

  // If the console can perform coloured output, we should allow it.
  err = start_color();
  if (err == ERR) {
    return EFAULT;
  }
  P(pApplication)->hasColors = has_colors();
  P(pApplication)->canChangeColors = can_change_color();

  // Don't echo the keys as they're pressed to the screen.
  err = noecho();
  if (err == ERR) {
    return EFAULT;
  }

  // Don't display the cursor by default - this will be toggled later.
  err = curs_set(0);
  if (err == ERR) {
    return EFAULT;
  }

  return 0;
}

//------------------------------------------------------------------------------
static int qcurses_application_end (
  qcurses_application_t *               pApplication
) {
  int err;
  fputs("\033[?1003l\n", stdout);
  err = endwin();
  if (err == ERR) {
    return EFAULT;
  }
  return 0;
}

//------------------------------------------------------------------------------
int qcurses_application_run (
  qcurses_application_t *               pApplication
) {
  int err;

  err = qcurses_application_start(pApplication);
  if (err) {
    return err;
  }

  // Grab the terminal sizes without issuing a resize event.
  err = qcurses_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  do {
    err = qcurses_application_update(pApplication);
  } while (!err && !P(pApplication)->isQuitting);

  (void)qcurses_application_end(pApplication);

  return err;
}

//------------------------------------------------------------------------------
int qcurses_application_quit (
  qcurses_application_t *               pApplication
) {
  P(pApplication)->isQuitting = QCURSES_TRUE;
  return 0;
}

//------------------------------------------------------------------------------
int __qcurses_application_set_main_widget (
  qcurses_application_t *               pApplication,
  qcurses_widget_t *                    pWidget
) {
  // TODO: What to do if another widget is disconnected?
  pWidget->pParent = (qcurses_widget_t *)pApplication;
  P(pApplication)->pMainWidget = pWidget;
  return 0;
}

//------------------------------------------------------------------------------
int qcurses_application_set_status_bar (
  qcurses_application_t *               pApplication,
  qcurses_status_bar_t *                pStatusBar
) {
  // TODO: Fix.
  //pStatusBar->widget.pParent = &pApplication->widget;
  //pApplication->statusBar = pStatusBar;
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
