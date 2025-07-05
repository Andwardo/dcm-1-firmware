#
#  CMakeLists.txt
#
#  Created on: 2024-06-18
#  Edited on: 2025-07-05 (local time)
#      Author: Andwardo
#      Version: v8.2.18
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

# Automatically embed all static asset files
file(GLOB EMBED_ASSETS
    "src/*.html"
    "src/*.css"
    "src/*.js"
    "src/*.svg"
)

foreach(asset ${EMBED_ASSETS})
    get_filename_component(fname ${asset} NAME)
    message(STATUS "Embedding static asset: ${fname}")
    target_add_binary_data(${COMPONENT_TARGET} "src/${fname}" TEXT)
endforeach()
