# File: main/CMakeLists.txt

idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES
        app_logic
        esp32-wifi-manager
        mqtt_manager
        esp_event
        nvs_flash
        board_manager
        esp_netif
        esp_wifi
        esp_http_server
        protocomm
)
