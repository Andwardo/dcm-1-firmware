#
# File: components/mqtt_manager/CMakeLists.txt
# Description: Build script for the mqtt_manager component.
# Created on: 2025-06-25
# Edited on:  2025-06-29
# Version: v8.5.3
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(
    SRCS "mqtt_manager.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES 
        mqtt
)
