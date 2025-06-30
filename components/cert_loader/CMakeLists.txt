#
# File: components/cert_loader/CMakeLists.txt
#

idf_component_register(
    SRCS "src/cert_loader.c"
    INCLUDE_DIRS "include"
    REQUIRES spiffs
)

# Create SPIFFS image for the 'certs' partition using the contents of the spiffs folder
spiffs_create_partition_image(certs ${CMAKE_CURRENT_SOURCE_DIR}/spiffs FLASH_IN_PROJECT)
