#
# File: components/app_logic/CMakeLists.txt
#
# Created on: 12 June 2025 23:30:00
# Last edited on: 12 June 2025 23:30:00
#
# Version: 8.0.0
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
