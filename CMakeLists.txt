cmake_minimum_required(VERSION 3.16)

# Tell IDF where *your* CSV lives
set(PARTITION_TABLE_CUSTOM_TABLE_FILE "${CMAKE_SOURCE_DIR}/partitions/partition-table.csv")
set(PARTITION_TABLE_CUSTOM_TABLE_OFFSET 0x8000)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(main)
