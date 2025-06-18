idf_component_register(
    SRCS
        "wifi_manager.c"
        "http_app.c"

    INCLUDE_DIRS
        "include"

    # Public dependencies that our .h files include
    REQUIRES
        esp_wifi

    # Private dependencies that only our .c files use
    PRIV_REQUIRES
        esp_event
        esp_http_server
        esp_netif
        nvs_flash
        cJSON
        driver
        lwip

    EMBED_FILES
        index.html
        style.css
        code.js
)
