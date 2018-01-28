QCurses
=======

QCurses is a Qt-inspired C wrapper over top of NCurses text input mode.
This project contains prototype code to learn how to work with Curses,
as well as provide a proof-of-concept abstraction over-top of the existing API.

![QCurses Label Sample](docs/images/qcurses-label.gif)

About the Name
--------------

Pretty self-explanatory, QCurses is simply a Qt-like layer overtop of existing NCurses.
Since the input mode is traditionally known as "curses", we have dropped the N for the name.

Qt-like Curses = QCurses!

About the Project
-----------------

It's worth mentioning that QCurses is simply a proof-of-concept.
Because of this, I would not recommend QCurses for any kind of production environment.
I cannot promise continued development or support on the project, but will gladly review PRs.

Any terminal UI applications that I build will be made using QCurses.
However, I do not consider it a first-class project - updates will be made when needed.

**Here is a partial list of things which need addressed to make QCurses production-ready:**
+ Allow linking with wide character version of ncurses.
+ Provide an upstream QCursesConfig.cmake file for downstream projects.
+ Provide a stronger definition for recalculate/paint commands.
+ Make layouts their own type (not a widget), because they don't need to be.
+ Provide proper deconstruction of objects (currently, there is none).
+ Allow the user to disconnect a signal/slot.
+ Don't allow signal/slots to be implemented as arrays - this is wasteful!
+ Provide stronger support for more complex layout configurations.
+ Support focus, tab order, and other useful GUI features.
+ And of course, add more of the required widgets for a UI.

How to Build
------------

There are only two dependencies, NCurses and LT3.
NCurses is the baseline for the curses text output mode which QCurses uses.
LT3 is a set of low-level libraries to provide extra C features which the logic requires.

Given that the two dependencies are installed on the host system,
here is a sample script that will build QCurses and samples in Debug mode:

```bash
$ cd /path/to/build/dir
$ cmake /path/to/source/dir
$ make -j8
```

Credits
-------

QCurses is being developed independently by Trent Reed (2017).
