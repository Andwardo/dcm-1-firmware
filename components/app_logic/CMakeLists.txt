#
# File: components/app_logic/CMakeLists.txt
#
# Created on: 12 June 2025 10:40:00
# Last edited on: 12 June 2025 21:50:00
#
# Version: 7.6.2
#
# Author: R. Andrew Ballard (c) 2025
#

idf_component_register(SRCS "app_logic.c"
                    INCLUDE_DIRS "include"
                    REQUIRES
                        board_manager
                        esp32-wifi-manager
                        mqtt_manager
                        nvs_flash)
