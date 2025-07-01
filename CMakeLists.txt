#
# File: CMakeLists.txt (Project Root)
# Description: Top-level build script for the PianoGuard_DCM-1 project.
# Created on: 2025-06-25
# Edited on:  2025-07-01
# Version: v8.6.4
# Author: R. Andrew Ballard (c) 2025
#

cmake_minimum_required(VERSION 3.16)

# Register additional component directories
set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/components)

# Set a custom partition table
set(PARTITION_TABLE_CUSTOM_FILENAME ${CMAKE_SOURCE_DIR}/partitions/partition-table.csv)

# Include the ESP-IDF project macros AFTER setting variables
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PianoGuard_DCM-1)

# Create a flashable SPIFFS image from /spiffs_image and target the 'spiffs' partition
include($ENV{IDF_PATH}/tools/cmake/spiffs.cmake)
spiffs_create_partition_image(spiffs
    ${CMAKE_SOURCE_DIR}/spiffs_image
    FLASH_IN_PROJECT
)
