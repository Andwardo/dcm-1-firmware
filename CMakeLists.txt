#
# File: CMakeLists.txt (Project Root)
# Description: Top-level build script for the PianoGuard_DCM-1 project.
# Created on: 2025-06-25
# Edited on:  2025-06-30
# Version: v8.6.2
# Author: R. Andrew Ballard (c) 2025
#
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Add the local 'components' directory to the build path
set(EXTRA_COMPONENT_DIRS components)
    
# Point to the custom partition table
set(PARTITION_TABLE_CUSTOM_CSV "${CMAKE_SOURCE_DIR}/partitions/partition-table.csv")
    
# Create a flashable SPIFFS image from the web assets inside the wifi manager component
# and target it to the 'spiffs' partition defined in the partition table.
# include($ENV{IDF_PATH}/tools/cmake/spiffs.cmake)
# spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/components/esp32-wifi-manager/src FLASH_IN_PROJECT)

project(PianoGuard_DCM-1)
