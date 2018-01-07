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

#include "application.h"
#include <ncurses.h>
#include <ctype.h>

#ifdef    __cplusplus
extern "C" {
#endif // __cplusplus

////////////////////////////////////////////////////////////////////////////////
// Slate Private Implementations
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
SLATE_PIMPL_STRUCT(slate_application_t) {
  WINDOW *                              mainWindow;
  slate_bool_t                          isQuitting;
  slate_widget_t *                      pMainWidget;
  slate_menu_bar_t *                    pMenuBar;       // TODO: Implement.
  slate_status_bar_t *                  pStatusBar;     // TODO: Implement.
};

////////////////////////////////////////////////////////////////////////////////
// Slate Public Functions
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
#define SLATE_CASE(theirs, ours) case theirs: code = ours; break
static int slate_application_update_input (
  slate_application_t *                 pApplication
) {
  int value;
  slate_keycode_t code;

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
      code = (slate_keycode_t)value;
      break;
    case 'A' ... 'Z':
      code = (slate_keycode_t)tolower(value);
      break;

    // Standard ASCII range (mapped)
    SLATE_CASE('\x1B', SLATE_KEYCODE_ESCAPE);
    SLATE_CASE('`', SLATE_KEYCODE_BACKTICK);
    SLATE_CASE('~', SLATE_KEYCODE_BACKTICK);
    SLATE_CASE('!', SLATE_KEYCODE_1);
    SLATE_CASE('@', SLATE_KEYCODE_2);
    SLATE_CASE('#', SLATE_KEYCODE_3);
    SLATE_CASE('$', SLATE_KEYCODE_4);
    SLATE_CASE('%', SLATE_KEYCODE_5);
    SLATE_CASE('^', SLATE_KEYCODE_6);
    SLATE_CASE('&', SLATE_KEYCODE_7);
    SLATE_CASE('*', SLATE_KEYCODE_8);
    SLATE_CASE('(', SLATE_KEYCODE_9);
    SLATE_CASE(')', SLATE_KEYCODE_0);
    SLATE_CASE('-', SLATE_KEYCODE_MINUS);
    SLATE_CASE('_', SLATE_KEYCODE_MINUS);
    SLATE_CASE('=', SLATE_KEYCODE_EQUALS);
    SLATE_CASE('+', SLATE_KEYCODE_EQUALS);
    SLATE_CASE('\t', SLATE_KEYCODE_TAB);
    SLATE_CASE('[', SLATE_KEYCODE_LEFT_BRACKET);
    SLATE_CASE('{', SLATE_KEYCODE_LEFT_BRACKET);
    SLATE_CASE(']', SLATE_KEYCODE_RIGHT_BRACKET);
    SLATE_CASE('}', SLATE_KEYCODE_RIGHT_BRACKET);
    SLATE_CASE('\\', SLATE_KEYCODE_REVERSE_SOLIDUS);
    SLATE_CASE('|', SLATE_KEYCODE_REVERSE_SOLIDUS);
    SLATE_CASE(';', SLATE_KEYCODE_SEMICOLON);
    SLATE_CASE(':', SLATE_KEYCODE_SEMICOLON);
    SLATE_CASE('\'', SLATE_KEYCODE_SINGLE_QUOTE);
    SLATE_CASE('"', SLATE_KEYCODE_SINGLE_QUOTE);
    SLATE_CASE('\n', SLATE_KEYCODE_RETURN);
    SLATE_CASE(',', SLATE_KEYCODE_COMMA);
    SLATE_CASE('<', SLATE_KEYCODE_COMMA);
    SLATE_CASE('.', SLATE_KEYCODE_PERIOD);
    SLATE_CASE('>', SLATE_KEYCODE_PERIOD);
    SLATE_CASE('/', SLATE_KEYCODE_SOLIDUS);
    SLATE_CASE('?', SLATE_KEYCODE_SOLIDUS);
    SLATE_CASE(' ', SLATE_KEYCODE_SPACE);

    // Special ncurses keycodes
    SLATE_CASE(KEY_DOWN, SLATE_KEYCODE_DOWN);
    SLATE_CASE(KEY_UP, SLATE_KEYCODE_UP);
    SLATE_CASE(KEY_LEFT, SLATE_KEYCODE_LEFT);
    SLATE_CASE(KEY_RIGHT, SLATE_KEYCODE_RIGHT);
    SLATE_CASE(KEY_HOME, SLATE_KEYCODE_HOME);
    SLATE_CASE(KEY_BACKSPACE, SLATE_KEYCODE_BACKSPACE);
    SLATE_CASE(KEY_F(0), SLATE_KEYCODE_F0);
    SLATE_CASE(KEY_F(1), SLATE_KEYCODE_F1);
    SLATE_CASE(KEY_F(2), SLATE_KEYCODE_F2);
    SLATE_CASE(KEY_F(3), SLATE_KEYCODE_F3);
    SLATE_CASE(KEY_F(4), SLATE_KEYCODE_F4);
    SLATE_CASE(KEY_F(5), SLATE_KEYCODE_F5);
    SLATE_CASE(KEY_F(6), SLATE_KEYCODE_F6);
    SLATE_CASE(KEY_F(7), SLATE_KEYCODE_F7);
    SLATE_CASE(KEY_F(8), SLATE_KEYCODE_F8);
    SLATE_CASE(KEY_F(9), SLATE_KEYCODE_F9);
    SLATE_CASE(KEY_F(10), SLATE_KEYCODE_F10);
    SLATE_CASE(KEY_F(11), SLATE_KEYCODE_F11);
    SLATE_CASE(KEY_F(12), SLATE_KEYCODE_F12);
    SLATE_CASE(KEY_DC, SLATE_KEYCODE_DELETE);
    SLATE_CASE(KEY_IC, SLATE_KEYCODE_INSERT);
    SLATE_CASE(KEY_SF, SLATE_KEYCODE_DOWN);
    SLATE_CASE(KEY_SR, SLATE_KEYCODE_UP);
    SLATE_CASE(KEY_NPAGE, SLATE_KEYCODE_PAGE_DOWN);
    SLATE_CASE(KEY_PPAGE, SLATE_KEYCODE_PAGE_UP);
    SLATE_CASE(KEY_BTAB, SLATE_KEYCODE_TAB);
    SLATE_CASE(KEY_ENTER, SLATE_KEYCODE_RETURN);
    SLATE_CASE(KEY_END, SLATE_KEYCODE_END);
    SLATE_CASE(KEY_SDC, SLATE_KEYCODE_DELETE);
    SLATE_CASE(KEY_SEND, SLATE_KEYCODE_END);
    SLATE_CASE(KEY_SHOME, SLATE_KEYCODE_HOME);
    SLATE_CASE(KEY_SLEFT, SLATE_KEYCODE_LEFT);
    SLATE_CASE(KEY_SNEXT, SLATE_KEYCODE_PAGE_DOWN);
    SLATE_CASE(KEY_SPREVIOUS, SLATE_KEYCODE_PAGE_UP);
    SLATE_CASE(KEY_SRIGHT, SLATE_KEYCODE_RIGHT);

    // Unhandled (error)
    default:
      code = SLATE_KEYCODE_INVALID;
      mvprintw(0, 0, "Unhandled value: 0x%x (%d) (0%o)\n", value, value, value);
      break;
  }
  slate_widget_emit(pApplication, onKey, code, value);

  return 0;
}
#undef SLATE_CASE

//------------------------------------------------------------------------------
static int SLATECALL slate_application_recalculate (
  slate_application_t *                 pApplication,
  slate_region_t const *                pRegion
) {
  int err;

  // If the actual region has not changed, we can simply ignore the recalculate step.
  // This means a recalculate was requested, but that none of the math would change.
  if (slate_region_equal(&W(pApplication)->outerRegion, pRegion)) {
    return 0;
  }

  // Unlike other recalculate functions, application is special.
  // Application will ignore minimum/maximum size since it has no control over it.
  // Application also ignores size policies, instead always fixing to the size of the screen.
  // Because of this, in case the user erroneously changed these values, we should reset.
  slate_widget_mark_state(pApplication, SLATE_STATE_DIRTY_BIT);
  W(pApplication)->sizePolicy     = SLATE_POLICY_FIXED;
  W(pApplication)->minimumBounds  = slate_bounds(0, 0);
  W(pApplication)->maximumBounds  = slate_bounds(SLATE_INFINITE, SLATE_INFINITE);
  W(pApplication)->contentBounds  = pRegion->bounds;
  W(pApplication)->outerRegion    = *pRegion;

  // The central widget should fill the remaining space that menu/status aren't filling.
  // TODO: Right now, menu_bar and status_bar aren't implemented - so same as pRegion.
  if (slate_widget_check(P(pApplication)->pMainWidget)) {
    err = slate_widget_recalculate(P(pApplication)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
static int slate_application_paint (
  slate_application_t *                 pApplication,
  slate_region_t const *                pRegion
) {
  int err;

  // Paint the central widget with the remaining region of the terminal.
  // TODO: Why do we store the painterRegion in each widget if we are just going to pass it in again?
  //       Is there even a need for a recalculate step that is separate from painting?
  //       Should take some time to seriously consider this aspect of the design, because it doesn't make sense.
  if (slate_widget_check(P(pApplication)->pMainWidget)) {
    err = slate_widget_paint(P(pApplication)->pMainWidget, pRegion);
    if (err) {
      return err;
    }
  }

  slate_widget_unmark_dirty(pApplication);
  return 0;
}

//------------------------------------------------------------------------------
static int slate_application_update_resize (
  slate_application_t *                 pApplication
) {
  int err;
  int x, y;
  slate_region_t resize;

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
  slate_bool_t const screenSizeChanged = SLATE_BOOL(
    (W(pApplication)->contentBounds.columns != resize.bounds.columns) ||
    (W(pApplication)->contentBounds.rows    != resize.bounds.rows   )
  );
  if (!screenSizeChanged) {
    return 0;
  }

  // Update the last-known application screen sizes.
  slate_widget_emit(pApplication, onResize, &resize.bounds);

  // Recalculate the sizes of all the widgets within the application.
  err = slate_application_recalculate(pApplication, &resize);
  if (err) {
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
static int slate_application_update (
  slate_application_t *                 pApplication
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
    err = slate_application_update_input(pApplication);
  } while (!err);
  if (err != ENODATA) {
    return err;
  }

  // Handle a state-change to quit.
  if (P(pApplication)->isQuitting) {
    return 0;
  }

  // Process pending resize (if resized).
  err = slate_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  // Handle the visual update iff the application is marked dirty.
  if (slate_widget_is_dirty(pApplication)) {
    err = slate_application_paint(pApplication, &W(pApplication)->outerRegion);
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
  err = slate_application_update_input(pApplication);
  if (err == ERR) {
    return err;
  }

  return 0;
}

//------------------------------------------------------------------------------
int slate_create_application (
  slate_application_info_t const *      pCreateInfo,
  slate_application_t **                pApplication
) {
  int err;
  slate_widget_config_t widgetConfig;
  slate_application_t * application;

  // Configure the application as a widget for ease of use.
  widgetConfig.pAllocator = pCreateInfo->pAllocator;
  widgetConfig.widgetSize = sizeof(slate_application_t) + sizeof(struct SLATE_PIMPL_NAME(slate_application_t));
  widgetConfig.pfnDestroy = (slate_widget_destroy_pfn)&slate_destroy_application;
  widgetConfig.pfnRecalculate = (slate_widget_recalc_pfn)&slate_application_recalculate;
  widgetConfig.pfnPaint = (slate_widget_paint_pfn)&slate_application_paint;

  // Allocate the terminal UI application.
  err = slate_create_widget(
    &widgetConfig,
    &application
  );
  if (err) {
    return err;
  }

  // Grab the application private implementation pointer.
  P(application) = (struct SLATE_PIMPL_NAME(slate_application_t)*)&application[1];

  // Return the application to the caller.
  *pApplication = application;
  return 0;
}

//------------------------------------------------------------------------------
void slate_destroy_application (
  slate_application_t *                 pApplication
) {
  // TODO: Destroy application properly.
  (void)pApplication;
}

//------------------------------------------------------------------------------
static int slate_application_start (
  slate_application_t *                 pApplication
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
static int slate_application_end (
  slate_application_t *                 pApplication
) {
  int err;
  err = endwin();
  if (err == ERR) {
    return EFAULT;
  }
  return 0;
}

//------------------------------------------------------------------------------
int slate_application_run (
  slate_application_t *                 pApplication
) {
  int err;

  err = slate_application_start(pApplication);
  if (err) {
    return err;
  }

  // Grab the terminal sizes without issuing a resize event.
  err = slate_application_update_resize(pApplication);
  if (err) {
    return err;
  }

  do {
    err = slate_application_update(pApplication);
  } while (!err && !P(pApplication)->isQuitting);

  (void)slate_application_end(pApplication);

  return err;
}

//------------------------------------------------------------------------------
int slate_application_quit (
  slate_application_t *                 pApplication
) {
  P(pApplication)->isQuitting = SLATE_TRUE;
  return 0;
}

//------------------------------------------------------------------------------
int __slate_application_set_main_widget (
  slate_application_t *                 pApplication,
  slate_widget_t *                      pWidget
) {
  // TODO: What to do if another widget is disconnected?
  pWidget->pParent = (slate_widget_t *)pApplication;
  P(pApplication)->pMainWidget = pWidget;
  return 0;
}

//------------------------------------------------------------------------------
int slate_application_set_status_bar (
  slate_application_t *                 pApplication,
  slate_status_bar_t *                  pStatusBar
) {
  // TODO: Fix.
  //pStatusBar->widget.pParent = &pApplication->widget;
  //pApplication->statusBar = pStatusBar;
  return 0;
}

#ifdef    __cplusplus
}
#endif // __cplusplus
