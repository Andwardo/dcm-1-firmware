cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(PianoGuard_DCM-1)

set(PARTITION_TABLE_CUSTOM_CSV ${CMAKE_SOURCE_DIR}/partitions/partition-table.csv)

# --- Manual SPIFFS Image Generation and Flashing ---

# 1. Define variables for the certs partition.
set(CERTS_PARTITION_NAME "certs")
set(CERTS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/certs")
set(CERTS_BINARY_IMAGE "${CMAKE_BINARY_DIR}/certs.bin")

# 2. Add a custom build step that runs the spiffsgen.py script.
add_custom_command(
    OUTPUT ${CERTS_BINARY_IMAGE}
    COMMAND ${PYTHON} ${IDF_PATH}/components/spiffs/spiffsgen.py
            24576
            ${CERTS_SOURCE_DIR}
            ${CERTS_BINARY_IMAGE}
    DEPENDS ${CERTS_SOURCE_DIR}
    COMMENT "Generating certs SPIFFS image from ${CERTS_SOURCE_DIR}"
    VERBATIM
)

# 3. Create a custom target to execute the command above.
add_custom_target(spiffs_certs_image_target DEPENDS ${CERTS_BINARY_IMAGE})

# 4. Add the generated binary to the list of files to be flashed by 'idf.py flash'.
idf_build_set_property(FLASH_IN_PROJECT "1" APPEND)
idf_build_set_property(FLASH_ARGS "${CERTS_PARTITION_NAME} ${CERTS_BINARY_IMAGE}" APPEND)
