cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PianoGuard_DCM-1)

set(PARTITION_TABLE_CUSTOM_CSV ${CMAKE_SOURCE_DIR}/partitions/partition-table.csv)
