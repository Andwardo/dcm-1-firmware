cmake_minimum_required(VERSION 3.16)

# bring in the IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

set(IDF_TARGET "esp32s3" CACHE STRING "" FORCE)

# name the project — do _not_ list COMPONENTS here
project(PianoGuard_DCM-1)
