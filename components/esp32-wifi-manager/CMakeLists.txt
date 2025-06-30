#
# File: components/esp32-wifi-manager/CMakeLists.txt
# Description: Corrected build script for the esp32-wifi-manager component.
# Created on: 2025-06-25
# Edited on:  2025-06-29
# Version: v8.5.2
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(
    SRCS
        "wifi_manager.c"
        "src/http_app.c"
        "src/dns_server.c"
        "src/json.c"
        "src/nvs_sync.c"
    INCLUDE_DIRS
        "."
	"src"  # <-- Corrected to point to the 'src' directory
    REQUIRES
        esp_wifi
    PRIV_REQUIRES
        esp_event
        esp_http_server
        esp_netif
        nvs_flash
        cJSON
        lwip
	spiffs
)
