# CMakeLists.txt — wifi_manager
#
# Final version with proper embed ordering
# Author: R. Andrew Ballard (c) 2025
# Version: v8.7.10
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

# Embed static assets directly (no "data/" prefix!)
target_add_binary_data(${COMPONENT_LIB} "index.html" TEXT)
target_add_binary_data(${COMPONENT_LIB} "style.css" TEXT)
target_add_binary_data(${COMPONENT_LIB} "code.js" TEXT)
target_add_binary_data(${COMPONENT_LIB} "lock.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "wifi0.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "wifi1.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "wifi2.svg" BINARY)
target_add_binary_data(${COMPONENT_LIB} "wifi3.svg" BINARY)
