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

    EMBED_TXTFILES
        "src/portal/index.html"
        "src/portal/style.css"
)
