################################################################################
# Project: slate
# Legal  : All content 2017 Trent Reed, all rights reserved.
# Author : Trent Reed
# About  : The main CMake script.
################################################################################

cmake_minimum_required(VERSION 3.5)
include(cmake/submodules.cmake)

################################################################################
# Project Initialization
################################################################################
project(slate C)

################################################################################
# Project Configuration
################################################################################
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build Type (default: Debug)")
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_SOURCE_DIR}/cmake/share;${CMAKE_SOURCE_DIR}/cmake/Modules")
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
include(Common)
include(Versioning)

################################################################################
# Project Dependencies
################################################################################
set(CURSES_NEED_NCURSES TRUE)
find_package(Curses REQUIRED)

################################################################################
# Project Includes
################################################################################
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CURSES_INCLUDE_DIRS})

################################################################################
# Project Options
################################################################################
# TODO: For now, while in early development, let's not turn on warnings.
option(SLATE_BUILD_SAMPLES "Whether or not to build the sample applications." ON)
option(SLATE_FULL_WARNINGS "Whether or not to compile with full warnings enabled." OFF)

# Set full warnings for the compiler if requested.
if(SLATE_FULL_WARNINGS)
  include(FullWarnings)
endif()

################################################################################
# Core Library
################################################################################
set(SLATE_SRC
  slate/slate.c
  slate/slate.h
)

add_library(slate ${SLATE_SRC})
target_link_libraries(slate ${CURSES_LIBRARIES})

################################################################################
# Misc. Binaries and Drivers
################################################################################

if(SLATE_BUILD_SAMPLES)
  add_subdirectory(samples)
endif()
