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
#include <ncurses.h>
#include <ctype.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Application Implementations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
QCURSES_PIMPL_STRUCT(qcurses_application_t) {
  WINDOW *                              mainWindow;
  qcurses_bool_t                        isQuitting;
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
  qcurses_keycode_t code;

  // TODO: Add the shift/Ctrl/Alt key modifiers - this could be useful to some.
  //       https://stackoverflow.com/questions/9750588/how-to-get-ctrl-shift-or-alt-with-getch-ncurses
  value = getch();
  switch (value) {

    // Ignored input events
    case ERR:
    case KEY_EVENT:
    case KEY_MOUSE:
    case KEY_RESIZE:
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
static int QCURSESCALL qcurses_application_recalculate (
  qcurses_application_t *               pApplication,
  qcurses_region_t const *              pRegion
) {
  int err;

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (qcurses_region_equal(&W(pApplication)->outerRegion, pRegion)) {
    return 0;
  }

  // Unlike other recalculate functions, application is special.
  // Application will ignore minimum/maximum size since it has no control over it.
  // Application also ignores size policies, instead always fixing to the size of the screen.
  // Because of this, in case the user erroneously changed these values, we should reset.
  qcurses_widget_mark_state(pApplication, QCURSES_STATE_DIRTY_BIT);
  W(pApplication)->sizePolicy     = QCURSES_POLICY_FIXED;
  W(pApplication)->minimumBounds  = qcurses_bounds(0, 0);
  W(pApplication)->maximumBounds  = qcurses_bounds(QCURSES_INFINITE, QCURSES_INFINITE);
  W(pApplication)->contentBounds  = pRegion->bounds;
  W(pApplication)->outerRegion    = *pRegion;

  // The central widget should fill the remaining space that menu/status aren't filling.
  // TODO: Right now, menu_bar and status_bar aren't implemented - so same as pRegion.
  if (qcurses_widget_check(P(pApplication)->pMainWidget)) {
    err = qcurses_widget_recalculate(P(pApplication)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
static int qcurses_application_paint (
  qcurses_application_t *               pApplication,
  qcurses_region_t const *              pRegion
) {
  int err;

  // Paint the central widget with the remaining region of the terminal.
  // TODO: Why do we store the painterRegion in each widget if we are just going to pass it in again?
  //       Is there even a need for a recalculate step that is separate from painting?
  //       Should take some time to seriously consider this aspect of the design, because it doesn't make sense.
  if (qcurses_widget_check(P(pApplication)->pMainWidget)) {
    err = qcurses_widget_paint(P(pApplication)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  qcurses_widget_unmark_dirty(pApplication);
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

  // If the screen bounds is less than the minimum allowed,
  // we should first restrict the bounds of the resize event.
  // TODO: Is this needed? We just throw this data away in recalc anyways...
  if (resize.bounds.columns < W(pApplication)->minimumBounds.columns) {
    resize.bounds.columns = W(pApplication)->minimumBounds.columns;
  }
  if (resize.bounds.rows < W(pApplication)->minimumBounds.rows) {
    resize.bounds.rows = W(pApplication)->minimumBounds.rows;
  }

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
  // TODO: Handle the mouse/resize events through input (more efficient).
  // TODO: We can only use this nodelay pattern if we can inject custom events.
  //       With more active UIs this may not be a possible pattern, may reconsider.
  err = nodelay(P(pApplication)->mainWindow, TRUE);
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
  err = qcurses_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  // Handle the visual update iff the application is marked dirty.
  if (qcurses_widget_is_dirty(pApplication)) {
    err = qcurses_application_paint(pApplication, &W(pApplication)->outerRegion);
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
  err = nodelay(P(pApplication)->mainWindow, FALSE);
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
  widgetConfig.widgetSize = sizeof(qcurses_application_t) + sizeof(struct QCURSES_PIMPL_NAME(qcurses_application_t));
  widgetConfig.pfnDestroy = (qcurses_widget_destroy_pfn)&qcurses_destroy_application;
  widgetConfig.pfnRecalculate = (qcurses_widget_recalc_pfn)&qcurses_application_recalculate;
  widgetConfig.pfnPaint = (qcurses_widget_paint_pfn)&qcurses_application_paint;

  // Allocate the terminal UI application.
  err = qcurses_create_widget(
    &widgetConfig,
    &application
  );
  if (err) {
    return err;
  }

  // Grab the application private implementation pointer.
  P(application) = (struct QCURSES_PIMPL_NAME(qcurses_application_t)*)&application[1];

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
  P(pApplication)->mainWindow = initscr();
  if (!P(pApplication)->mainWindow) {
    return EFAULT;
  }

  // Attempt to enable the special keys (arrows, function, etc.)
  err = keypad(stdscr, TRUE);
  if (err == ERR) {
    return EFAULT;
  }

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
