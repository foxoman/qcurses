################################################################################
# Project: QCurses
# Legal  : All content 2017 Trent Reed, all rights reserved.
# Author : Trent Reed
# About  : Sample applications using the qcurses library.
################################################################################

################################################################################
# canvas
################################################################################
set(SAMPLE_CANVAS_SRC
  canvas/main.c
)
add_executable(canvas ${SAMPLE_CANVAS_SRC})
target_link_libraries(canvas qcurses)

################################################################################
# empty
################################################################################
set(SAMPLE_EMPTY_SRC
  empty/main.c
)
add_executable(empty ${SAMPLE_EMPTY_SRC})
target_link_libraries(empty qcurses)

################################################################################
# label
################################################################################
set(SAMPLE_LABEL_SRC
  label/main.c
)
add_executable(label ${SAMPLE_LABEL_SRC})
target_link_libraries(label qcurses)

################################################################################
# layout
################################################################################
set(SAMPLE_LAYOUT_SRC
  layout/main.c
)
add_executable(layout ${SAMPLE_LAYOUT_SRC})
target_link_libraries(layout qcurses)

################################################################################
# status_bar
################################################################################
# TODO: Currently commented-out until I am able to finish refactoring.
#set(SAMPLE_STATUS_BAR_SRC
#  status_bar/main.c
#)
#add_executable(status_bar ${SAMPLE_STATUS_BAR_SRC})
#target_link_libraries(status_bar qcurses)
