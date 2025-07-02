cmake_minimum_required(VERSION 3.16)

# Register additional component directories
set(EXTRA_COMPONENT_DIRS ${CMAKE_SOURCE_DIR}/components)

# Set a custom partition table
set(PARTITION_TABLE_CUSTOM_FILENAME ${CMAKE_SOURCE_DIR}/partitions/partition-table.csv)

# MUST BE LAST: This defines the actual project and loads ESP-IDF macros
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PianoGuard_DCM-1)
