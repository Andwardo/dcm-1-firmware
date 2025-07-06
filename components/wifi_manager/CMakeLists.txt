# CMakeLists.txt — wifi_manager
#
# Final version with proper embed ordering and asset path resolution
# Author: R. Andrew Ballard (c) 2025
# Version: v8.7.11
#

idf_component_register(
    SRCS
        "wifi_manager.c"
        "src/http_app.c"
        "src/dns_server.c"
        'src/wifi_manager.c"
    INCLUDE_DIRS
        "."
        "src"
        "include"
    REQUIRES
        esp_wifi
        esp_http_server
    PRIV_REQUIRES
        cJSON
        esp_event
        esp_netif
        lwip
        mqtt_manager
        nvs_flash
        vfs
    PRIV_INCLUDE_DIRS
        "$ENV{IDF_PATH}/components/wifi_provisioning/include"
)

# Use absolute paths to avoid silent failures
set(WIFI_MANAGER_ASSETS
    "${CMAKE_CURRENT_SOURCE_DIR}/index.html"
    "${CMAKE_CURRENT_SOURCE_DIR}/style.css"
    "${CMAKE_CURRENT_SOURCE_DIR}/code.js"
    "${CMAKE_CURRENT_SOURCE_DIR}/lock.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/wifi0.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/wifi1.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/wifi2.svg"
    "${CMAKE_CURRENT_SOURCE_DIR}/wifi3.svg"
)

# Embed each static asset with correct type
foreach(asset IN LISTS WIFI_MANAGER_ASSETS)
    get_filename_component(fname "${asset}" NAME)
    if(fname MATCHES "\\.svg$")
        target_add_binary_data(${COMPONENT_LIB} "${asset}" BINARY)
    else()
        target_add_binary_data(${COMPONENT_LIB} "${asset}" TEXT)
    endif()
endforeach()
