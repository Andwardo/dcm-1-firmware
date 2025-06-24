idf_component_register(
    SRCS
        # Source files for the main component, sorted alphabetically
        "cert_loader.c"
        "httpd_server.c"
        "main.c"
        "sensors.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES
        # Project-specific components
        app_logic
        board_manager
        mqtt_manager

        # Standard ESP-IDF components, sorted alphabetically
        cJSON
        esp_event
        esp_http_server
        esp_netif
        esp_system
        esp_wifi
        log
        mqtt      # Added explicit dependency for MQTT client
        nvs_flash
        spiffs
)

# --- Embed Certificate Files ---
# This method directly compiles the certificate data into the application binary.
# It is the most reliable method for including file data in the firmware.
# This assumes a 'certs' directory exists inside the 'main' directory.
target_add_binary_data(${COMPONENT_TARGET} "certs/root_ca.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/client.crt"  TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/client.key"  TEXT)
