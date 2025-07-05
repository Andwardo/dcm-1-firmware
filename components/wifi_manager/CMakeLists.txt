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

# Manually embed static web resources (correct path and mode)
target_add_binary_data(${COMPONENT_LIB} "src/index.html" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/style.css" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/code.js" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/lock.svg" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/wifi0.svg" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/wifi1.svg" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/wifi2.svg" TEXT)
target_add_binary_data(${COMPONENT_LIB} "src/wifi3.svg" TEXT)
