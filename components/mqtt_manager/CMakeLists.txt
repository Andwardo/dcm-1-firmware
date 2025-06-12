#
# File: components/mqtt_manager/CMakeLists.txt
#
# Created on: 12 June 2025 09:10:00
# Last edited on: 12 June 2025 09:10:00
#
# Version: 7.5.0
#
# Author: R. Andrew Ballard (c) 2025
#

idf_component_register(SRCS "mqtt_manager.c"
                    INCLUDE_DIRS "include"
                    REQUIRES mqtt esp-tls)
