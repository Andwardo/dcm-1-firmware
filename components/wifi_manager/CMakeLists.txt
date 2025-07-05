# CMakeLists.txt — wifi_manager
#
# Final version with proper embed ordering
# Author: R. Andrew Ballard (c) 2025
# Version: v8.7.9
#
idf_component_register(
    SRCS
        "wifi_manager.c"
        "src/http_app.c"
        "src/dns_server.c"
    INCLUDE_DIRS
        "."
        "src"
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
)

# Embed static assets as binary blobs (for use with asm("_binary_*") symbols)
target_add_binary_data(${COMPONENT_LIB} "data/index.html" TEXT)
target_add_binary_data(${COMPONENT_LIB} "data/style.css" TEXT)
target_add_binary_data(${COMPONENT_LIB} "data/code.js" TEXT)
target_add_binary_data(${COMPONENT_LIB} "data/lock.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "data/wifi0.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "data/wifi1.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "data/wifi2.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "data/wifi3.svg" BINARY)
