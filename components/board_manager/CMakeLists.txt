#
# File: components/board_manager/CMakeLists.txt
#
# Created on: 12 June 2025 23:30:00
# Last edited on: 12 June 2025 23:30:00
#
# Version: 8.0.0
#
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(SRCS "board_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver)
