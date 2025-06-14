idf_component_register(
    SRCS "wifi_manager.c"
    INCLUDE_DIRS "."      # wifi_manager.h must be here
    PRIV_REQUIRES provisioning_manager wifi_provisioning esp_event
)
