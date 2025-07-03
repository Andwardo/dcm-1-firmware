# components/app_logic/CMakeLists.txt
#
# Defines the build rules for the app_logic component.
# Ensures necessary source files and dependencies are registered.

idf_component_register(
    SRCS          "app_logic.c"
    INCLUDE_DIRS  "include"
    REQUIRES      freertos
                  log
                  cJSON
                  board_manager
                  mqtt_manager
                  wifi_manager
)
