#
# File: main/CMakeLists.txt
# Description: Build script for the main application component.
# Created on: 2025-06-25
# Edited on:  2025-06-30
# Version: v8.5.7
# Author: R. Andrew Ballard (c) 2025
# Removed the cert_loader from PRIV_REQUIRES
#

idf_component_register(
    SRCS
        "main.c"
    INCLUDE_DIRS
        "."
    PRIV_REQUIRES
        nvs_flash
        app_logic
        board_manager
        wifi_manager
        mqtt_manager
)
