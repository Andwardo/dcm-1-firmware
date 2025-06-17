idf_component_register(
    SRCS
        "wifi_manager.c"
        "http_app.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        esp_event
        esp_http_server
        esp_netif
        esp_wifi
        nvs_flash
        cJSON           # CORRECTED back to match your directory name
        driver
        lwip

    EMBED_FILES
        index.html
        style.css
        code.js
)
