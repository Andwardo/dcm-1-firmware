idf_component_register(
    SRCS
        # Source files for the main component
        "main.c"
        "httpd_server.c"
        "sensors.c"
        "cert_loader.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        # Project-specific components
        app_logic
        board_manager
        mqtt_manager  # <-- Correctly listed as a component dependency

        # Standard ESP-IDF components
        cJSON
        esp_event
        esp_http_server
        esp_netif
        esp_system
        esp_wifi
        log
        mqtt
        nvs_flash
        spiffs
)

# Embed the certificate files directly into the application binary as data
target_add_binary_data(${COMPONENT_TARGET} "certs/root_ca.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/client.crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/client.key" TEXT)
