# Root CMakeLists.txt for PianoGuard_DCM-1
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PianoGuard_DCM-1)

set(PARTITION_TABLE_CUSTOM_CSV "${CMAKE_SOURCE_DIR}/partitions/partition-table.csv")
set(EXTRA_COMPONENT_DIRS components)
spiffs_create_partition_image(spiffs ${CMAKE_SOURCE_DIR}/main/assets FLASH_IN_PROJECT)
