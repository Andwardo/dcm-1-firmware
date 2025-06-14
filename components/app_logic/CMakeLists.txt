idf_component_register(
  SRCS         "app_logic.c"
  INCLUDE_DIRS "."
  REQUIRES     board_manager
               wifi_manager
               mqtt_manager
               nvs_flash
)
