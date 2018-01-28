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

#include "qapplication.h"
#include "detail/qpainter.inl"
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
QPIMPL_STRUCT(qapplication_t) {
  qbool_t                               isQuitting;
  qbool_t                               hasMouse;
  qbool_t                               hasColors;
  qbool_t                               canChangeColors;
  qcoord_t                              mouseCoord;
  mmask_t                               mouseEvents;
  qregion_t                             screenRegion;
  qmouse_t                              stickyMouseState;
  qpainter_t                            painter;
  qwidget_t *                           pMainWidget;
  qmenu_bar_t *                         pMenuBar;       // TODO: Implement.
  qstatus_bar_t *                       pStatusBar;     // TODO: Implement.
};

////////////////////////////////////////////////////////////////////////////////
// Application Callbacks
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QRECALC(
  qapplication_recalculate,
  qapplication_t *                      pThis,
  qregion_t const *                     pRegion
) {
  int err;
  char * pNewBuffer;

  // Update the application's painter instance.
  QP(pThis)->painter.boundary = pRegion->bounds;
  if (QP(pThis)->painter.maxBounds.columns < pRegion->bounds.columns) {
    pNewBuffer = qreallocate(
      QW(pThis)->pAllocator,
      QP(pThis)->painter.pClearBrush,
      pRegion->bounds.columns
    );
    if (!pNewBuffer) {
      return ENOMEM;
    }
    memset(pNewBuffer, ' ', pRegion->bounds.columns);
    QP(pThis)->painter.pClearBrush = pNewBuffer;
    QP(pThis)->painter.maxBounds.columns = pRegion->bounds.columns;
  }
  if (QP(pThis)->painter.maxBounds.rows < pRegion->bounds.rows) {
    QP(pThis)->painter.maxBounds.rows = pRegion->bounds.rows;
  }

  // Unlike other recalculate functions, application is special.
  // Application will ignore minimum/maximum size since it has no control over it.
  // Application also ignores size policies, instead always fixing to the size of the screen.
  // Because of this, in case the user erroneously changed these values, we should reset.
  qwidget_mark_state(pThis, QSTATE_DIRTY_BIT);
  QW(pThis)->sizePolicy     = QPOLICY_FIXED;
  QW(pThis)->minimumBounds  = qbounds(0, 0);
  QW(pThis)->maximumBounds  = qbounds(QINFINITE, QINFINITE);
  QW(pThis)->contentBounds  = pRegion->bounds;
  QW(pThis)->outerRegion    = *pRegion;

  // The central widget should fill the remaining space that menu/status aren't filling.
  // TODO: Right now, menu_bar and status_bar aren't implemented - so same as pRegion.
  if (qwidget_check(QP(pThis)->pMainWidget)) {
    err = qwidget_recalculate(QP(pThis)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
QPAINTER(
  qapplication_paint,
  qapplication_t *                      pThis,
  qpainter_t *                          pPainter
) {
  int err;

  // Paint the central widget with the remaining region of the terminal.
  if (qwidget_check(QP(pThis)->pMainWidget)) {
    err = qwidget_paint(QP(pThis)->pMainWidget, pPainter);
    if (err) {
      return err;
    }
  }

  qwidget_unmark_dirty(pThis);
  return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Application Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
static int qapplication_update_event (
  qapplication_t *                      pApplication
) {
  return 0;
}

//------------------------------------------------------------------------------
static int qapplication_update_resize (
  qapplication_t *                      pApplication
) {
  int err;
  int x, y;
  qregion_t resize;

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
  qbool_t const screenSizeChanged = QBOOL(
    (QW(pApplication)->contentBounds.columns != resize.bounds.columns) ||
    (QW(pApplication)->contentBounds.rows    != resize.bounds.rows   )
  );
  if (!screenSizeChanged) {
    return 0;
  }

  // Update the last-known application screen sizes.
  // TODO: Really, find out what to do about this resize/recalculate thing.
  //       Maybe this needs to be sketched out and designed, this is annoying.
  QP(pApplication)->screenRegion = resize;
  qwidget_mark_dirty(pApplication);
  qwidget_emit(pApplication, resize, &resize.bounds);

  return 0;
}

//------------------------------------------------------------------------------
static int qapplication_update_mouse (
  qapplication_t *                      pApplication
) {
  MEVENT mouseEvent;
  qmouse_t mouseState;

  // Attempt to grab the mouse event, and fail otherwise.
  if (getmouse(&mouseEvent) != OK) {
    return EFAULT;
  }

  // Handle sticky key state transitions.
  mouseState = 0;
  if (mouseEvent.bstate & BUTTON1_PRESSED) {
    mouseState |= QMOUSE_LEFT_PRESSED_BIT;
    QP(pApplication)->stickyMouseState |= QMOUSE_LEFT_PRESSED_BIT;
  }
  if (mouseEvent.bstate & BUTTON1_RELEASED) {
    mouseState |= QMOUSE_LEFT_RELEASED_BIT;
    QP(pApplication)->stickyMouseState &= ~QMOUSE_LEFT_PRESSED_BIT;
  }
  if (mouseEvent.bstate & BUTTON2_PRESSED) {
    mouseState |= QMOUSE_MIDDLE_PRESSED_BIT;
    QP(pApplication)->stickyMouseState |= QMOUSE_MIDDLE_PRESSED_BIT;
  }
  if (mouseEvent.bstate & BUTTON2_RELEASED) {
    mouseState |= QMOUSE_MIDDLE_RELEASED_BIT;
    QP(pApplication)->stickyMouseState &= ~QMOUSE_MIDDLE_PRESSED_BIT;
  }
  if (mouseEvent.bstate & BUTTON3_PRESSED) {
    mouseState |= QMOUSE_RIGHT_PRESSED_BIT;
    QP(pApplication)->stickyMouseState |= QMOUSE_RIGHT_PRESSED_BIT;
  }
  if (mouseEvent.bstate & BUTTON3_RELEASED) {
    mouseState |= QMOUSE_RIGHT_RELEASED_BIT;
    QP(pApplication)->stickyMouseState &= ~QMOUSE_RIGHT_PRESSED_BIT;
  }
  mouseState |= QP(pApplication)->stickyMouseState;

  // Handle the mouse coordinate information.
  // Sometimes this mouse event can trigger outside of the valid screen bounds.
  // TODO: Handle z (mousewheel).
  QP(pApplication)->mouseCoord = qcoord(
    QMIN((size_t)mouseEvent.x, QP(pApplication)->painter.boundary.columns - 1),
    QMIN((size_t)mouseEvent.y, QP(pApplication)->painter.boundary.rows    - 1)
  );

  // Emit the mouse event.
  qwidget_emit(
    pApplication,
    on_mouse,
    &QP(pApplication)->mouseCoord,
    mouseState
  );

  return 0;
}

//------------------------------------------------------------------------------
#define QCASE(theirs, ours) case theirs: code = ours; break
static int qapplication_update_input (
  qapplication_t *                      pApplication
) {
  int value;
  qkey_t code;

  // TODO: Add the shift/Ctrl/Alt key modifiers - this could be useful to some.
  //       https://stackoverflow.com/questions/9750588/how-to-get-ctrl-shift-or-alt-with-getch-ncurses
  value = wgetch(QP(pApplication)->painter.pWindow);
  switch (value) {

    // Non-blocking results
    case ERR:
      return ENODATA;

    // Special code range
    case KEY_EVENT:
      return qapplication_update_event(pApplication);
    case KEY_MOUSE:
      return qapplication_update_mouse(pApplication);
    case KEY_RESIZE:
      return qapplication_update_resize(pApplication);

    // Standard ASCII range
    case 'a' ... 'z':
    case '0' ... '9':
      code = (qkey_t)value;
      break;
    case 'A' ... 'Z':
      code = (qkey_t)tolower(value);
      break;

    // Standard ASCII range (mapped)
    QCASE('\x1B', QKEY_ESCAPE);
    QCASE('`', QKEY_BACKTICK);
    QCASE('~', QKEY_BACKTICK);
    QCASE('!', QKEY_1);
    QCASE('@', QKEY_2);
    QCASE('#', QKEY_3);
    QCASE('$', QKEY_4);
    QCASE('%', QKEY_5);
    QCASE('^', QKEY_6);
    QCASE('&', QKEY_7);
    QCASE('*', QKEY_8);
    QCASE('(', QKEY_9);
    QCASE(')', QKEY_0);
    QCASE('-', QKEY_MINUS);
    QCASE('_', QKEY_MINUS);
    QCASE('=', QKEY_EQUALS);
    QCASE('+', QKEY_EQUALS);
    QCASE('\t', QKEY_TAB);
    QCASE('[', QKEY_LEFT_BRACKET);
    QCASE('{', QKEY_LEFT_BRACKET);
    QCASE(']', QKEY_RIGHT_BRACKET);
    QCASE('}', QKEY_RIGHT_BRACKET);
    QCASE('\\', QKEY_REVERSE_SOLIDUS);
    QCASE('|', QKEY_REVERSE_SOLIDUS);
    QCASE(';', QKEY_SEMICOLON);
    QCASE(':', QKEY_SEMICOLON);
    QCASE('\'', QKEY_SINGLE_QUOTE);
    QCASE('"', QKEY_SINGLE_QUOTE);
    QCASE('\n', QKEY_RETURN);
    QCASE(',', QKEY_COMMA);
    QCASE('<', QKEY_COMMA);
    QCASE('.', QKEY_PERIOD);
    QCASE('>', QKEY_PERIOD);
    QCASE('/', QKEY_SOLIDUS);
    QCASE('?', QKEY_SOLIDUS);
    QCASE(' ', QKEY_SPACE);

    // Special ncurses keycodes
    QCASE(KEY_DOWN, QKEY_DOWN);
    QCASE(KEY_UP, QKEY_UP);
    QCASE(KEY_LEFT, QKEY_LEFT);
    QCASE(KEY_RIGHT, QKEY_RIGHT);
    QCASE(KEY_HOME, QKEY_HOME);
    QCASE(KEY_BACKSPACE, QKEY_BACKSPACE);
    QCASE(KEY_F(0), QKEY_F0);
    QCASE(KEY_F(1), QKEY_F1);
    QCASE(KEY_F(2), QKEY_F2);
    QCASE(KEY_F(3), QKEY_F3);
    QCASE(KEY_F(4), QKEY_F4);
    QCASE(KEY_F(5), QKEY_F5);
    QCASE(KEY_F(6), QKEY_F6);
    QCASE(KEY_F(7), QKEY_F7);
    QCASE(KEY_F(8), QKEY_F8);
    QCASE(KEY_F(9), QKEY_F9);
    QCASE(KEY_F(10), QKEY_F10);
    QCASE(KEY_F(11), QKEY_F11);
    QCASE(KEY_F(12), QKEY_F12);
    QCASE(KEY_DC, QKEY_DELETE);
    QCASE(KEY_IC, QKEY_INSERT);
    QCASE(KEY_SF, QKEY_DOWN);
    QCASE(KEY_SR, QKEY_UP);
    QCASE(KEY_NPAGE, QKEY_PAGE_DOWN);
    QCASE(KEY_PPAGE, QKEY_PAGE_UP);
    QCASE(KEY_BTAB, QKEY_TAB);
    QCASE(KEY_ENTER, QKEY_RETURN);
    QCASE(KEY_END, QKEY_END);
    QCASE(KEY_SDC, QKEY_DELETE);
    QCASE(KEY_SEND, QKEY_END);
    QCASE(KEY_SHOME, QKEY_HOME);
    QCASE(KEY_SLEFT, QKEY_LEFT);
    QCASE(KEY_SNEXT, QKEY_PAGE_DOWN);
    QCASE(KEY_SPREVIOUS, QKEY_PAGE_UP);
    QCASE(KEY_SRIGHT, QKEY_RIGHT);

    // Unhandled (error)
    default:
      code = QKEY_INVALID;
      mvprintw(0, 0, "Unhandled value: 0x%x (%d) (0%o)\n", value, value, value);
      break;
  }
  qwidget_emit(pApplication, on_key, code, value);

  return 0;
}
#undef QCASE

//------------------------------------------------------------------------------
static int qapplication_update_input_blocking (
  qapplication_t *                      pApplication
) {
  int err;
  err = nodelay(QP(pApplication)->painter.pWindow, FALSE);
  if (err == ERR) {
    return EFAULT;
  }
  return qapplication_update_input(pApplication);
}

//------------------------------------------------------------------------------
static int qapplication_update_input_nonblocking (
  qapplication_t *                      pApplication
) {
  int err;
  err = nodelay(QP(pApplication)->painter.pWindow, TRUE);
  if (err == ERR) {
    return EFAULT;
  }
  do {
    err = qapplication_update_input(pApplication);
  } while (!err);
  return err;
}

//------------------------------------------------------------------------------
static int qapplication_update (
  qapplication_t *                      pApplication
) {
  int err;

  // Process all pending input (if available).
  err = qapplication_update_input_nonblocking(pApplication);
  if (err != ENODATA) {
    return err;
  }

  // Handle the visual update iff the application is marked dirty.
  if (qwidget_is_dirty(pApplication)) {
    err = qapplication_recalculate(pApplication, &QP(pApplication)->screenRegion);
    if (err) {
      return err;
    }
    err = qapplication_paint(pApplication, &QP(pApplication)->painter);
    if (err) {
      return err;
    }
    err = wrefresh(QP(pApplication)->painter.pWindow);
    if (err) {
      return EFAULT;
    }
  }

  // Handle a state-change to quit (do this before blocking input query).
  if (QP(pApplication)->isQuitting) {
    return 0;
  }

  // Block for any kind of update which might cause output to change.
  // No big deal if we don't process all input - head logic will capture the rest.
  err = qapplication_update_input_blocking(pApplication);
  if (err) {
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qcreate_application (
  qapplication_info_t const *           pCreateInfo,
  qapplication_t **                     pApplication
) {
  int err;
  qwidget_config_t widgetConfig;
  qapplication_t * application;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator     = pCreateInfo->pAllocator;
  widgetConfig.publicSize     = sizeof(qapplication_t);
  widgetConfig.privateSize    = sizeof(QPIMPL_STRUCT(qapplication_t));
  widgetConfig.pfnDestroy     = QDESTROY_PTR(qdestroy_application);
  widgetConfig.pfnRecalculate = QRECALC_PTR(qapplication_recalculate);
  widgetConfig.pfnPaint       = QPAINTER_PTR(qapplication_paint);

  // Allocate the terminal UI application.
  err = qcreate_widget(
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
void QCURSESCALL qdestroy_application (
  qapplication_t *                      pApplication
) {
  // TODO: Destroy application properly.
  (void)pApplication;
}

//------------------------------------------------------------------------------
static int qapplication_start (
  qapplication_t *                      pApplication
) {
  int err;

  // Construct the main window
  // TODO: Should probably add signal handlers to reset the screen state.
  QP(pApplication)->painter.pWindow = initscr();
  if (!QP(pApplication)->painter.pWindow) {
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
  QP(pApplication)->mouseEvents = mousemask(
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
  QP(pApplication)->hasMouse = has_mouse();

  // If the console can perform coloured output, we should allow it.
  err = start_color();
  if (err == ERR) {
    return EFAULT;
  }
  QP(pApplication)->hasColors = has_colors();
  QP(pApplication)->canChangeColors = can_change_color();

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
static int qapplication_end (
  qapplication_t *                      pApplication
) {
  int err;
  err = fputs("\033[?1003l\n", stdout);
  if (err == EOF) {
    return EFAULT;
  }
  err = endwin();
  if (err == ERR) {
    return EFAULT;
  }
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_run (
  qapplication_t *                      pApplication
) {
  int err;

  // Start the required application subsystems.
  err = qapplication_start(pApplication);
  if (err) {
    return err;
  }

  // Issue a fake "resize" event just to get things sized properly.
  // This also marks the screen as dirty so that we can draw it.
  err = qapplication_update_resize(pApplication);
  if (err) {
    return err;
  }

  // Perform our main update loop.
  do {
    err = qapplication_update(pApplication);
  } while (!err && !QP(pApplication)->isQuitting);

  // Stop the required application subsystems.
  err = qapplication_end(pApplication);
  if (err) {
    return err;
  }

  return err;
}

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_quit (
  qapplication_t *                      pApplication,
  int                                   code
) {
  QP(pApplication)->isQuitting = QTRUE;
  qwidget_emit(pApplication, quit, code);
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL __qapplication_set_main_widget (
  qapplication_t *                      pApplication,
  qwidget_t *                           pWidget
) {
  // TODO: What to do if another widget is disconnected?
  pWidget->pParent = (qwidget_t *)pApplication;
  QP(pApplication)->pMainWidget = pWidget;
  return 0;
}

//------------------------------------------------------------------------------
int QCURSESCALL qapplication_set_status_bar (
  qapplication_t *                      pApplication,
  qstatus_bar_t *                       pStatusBar
) {
  // TODO: Fix.
  //pStatusBar->widget.pParent = &pApplication->widget;
  //pApplication->statusBar = pStatusBar;
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
