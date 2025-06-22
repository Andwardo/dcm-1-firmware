cmake_minimum_required(VERSION 3.16)

# Pull in IDF’s project support
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# This must match your app’s name
project(PianoGuard_DCM-1)

# Package main/assets/ into a SPIFFS image and embed in flash
spiffs_create_partition_image(spiffs "${CMAKE_SOURCE_DIR}/main/assets" FLASH_IN_PROJECT)
