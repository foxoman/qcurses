################################################################################
# Project: slate
# Legal  : All content 2017 Trent Reed, all rights reserved.
# Author : Trent Reed
# About  : Sample applications using the slate library.
################################################################################

################################################################################
# status_bar
################################################################################
set(SAMPLE_STATUS_BAR_SRC
  status_bar/main.c
)
add_executable(status_bar ${SAMPLE_STATUS_BAR_SRC})
target_link_libraries(status_bar slate)
