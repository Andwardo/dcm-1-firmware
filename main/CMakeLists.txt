#
# File: main/CMakeLists.txt
#
# Created on: 11 June 2025 08:50:00
# Last edited on: 12 June 2025 21:40:00
#
# Version: 7.6.1
#
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash app_logic)
