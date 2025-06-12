#
# File: components/board_manager/CMakeLists.txt
#
# Created on: 12 June 2025 21:20:00
# Last edited on: 12 June 2025 21:40:00
#
# Version: 7.6.1
#
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(SRCS "board_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES driver)
