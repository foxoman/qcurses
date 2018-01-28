################################################################################
# Project: QCurses
# Legal  : All content 2017 Trent Reed, all rights reserved.
# Author : Trent Reed
# About  : The main CMake script.
################################################################################

cmake_minimum_required(VERSION 3.5)
include(cmake/submodules.cmake)

################################################################################
# Project Initialization
################################################################################
project(QCurses C)

################################################################################
# Project Configuration
################################################################################
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type (default: Debug)")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/share")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
include(Common)
include(Versioning)

################################################################################
# Project Dependencies
################################################################################
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)
find_package(Lt3 REQUIRED COMPONENTS String)

################################################################################
# Project Includes
################################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CURSES_INCLUDE_DIRS})

################################################################################
# Project Options
################################################################################
# TODO: For now, while in early development, let's not turn on warnings.
option(QCURSES_BUILD_SAMPLES "Whether or not to build the sample applications." ON)
option(QCURSES_FULL_WARNINGS "Whether or not to compile with full warnings enabled." OFF)

# Set full warnings for the compiler if requested.
if(QCURSES_FULL_WARNINGS)
  include(FullWarnings)
endif()

################################################################################
# Core Library
################################################################################
set(QCURSES_SRC
  qcurses/qapplication.c
  qcurses/qapplication.h
  qcurses/qarray.c
  qcurses/qarray.h
  qcurses/qlabel.c
  qcurses/qlabel.h
  qcurses/qlayout.c
  qcurses/qlayout.h
  qcurses/qmath.h
  qcurses/qmenu_bar.h
  qcurses/qpainter.c
  qcurses/qpainter.h
  qcurses/qcurses.c
  qcurses/qcurses.h
  qcurses/qstatus_bar.h
  qcurses/qwidget.c
  qcurses/qwidget.h
)

add_library(qcurses ${QCURSES_SRC})
target_link_libraries(qcurses ${CURSES_LIBRARIES} Lt3::String)

################################################################################
# Misc. Binaries and Drivers
################################################################################
if(QCURSES_BUILD_SAMPLES)
  add_subdirectory(samples)
endif()
