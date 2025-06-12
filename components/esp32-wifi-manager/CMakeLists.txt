#
# File: components/esp32-wifi-manager/CMakeLists.txt
#
# Created on: 12 June 2025 07:20:00
# Last edited on: 12 June 2025 07:20:00
#
# Version: 7.5.0
#
# Author: R. Andrew Ballard (c) 2025
#

set(EMBED_FILES
    style.css
    code.js
    index.html
)

idf_component_register(SRCS "wifi_manager.c" "http_app.c"
                    INCLUDE_DIRS "include"
                    EMBED_FILES "${EMBED_FILES}"
                    REQUIRES nvs_flash esp_wifi esp_http_server esp_netif esp_event json)
