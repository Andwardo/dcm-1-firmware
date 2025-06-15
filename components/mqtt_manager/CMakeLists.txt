idf_component_register(
    SRCS            "mqtt_manager.c"
    INCLUDE_DIRS    "include"
    PRIV_REQUIRES   mqtt esp-tls esp32-wifi-manager
    REQUIRES        esp_event
)
