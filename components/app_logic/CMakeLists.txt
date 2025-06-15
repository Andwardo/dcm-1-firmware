# components/app_logic/CMakeLists.txt
#
# Defines the build rules for the app_logic component.
# Ensures necessary source files and dependencies are registered.

idf_component_register(
    SRCS
        "app_logic.c"

    INCLUDE_DIRS
        "include"

    PRIV_REQUIRES esp32-wifi-manager mqtt_manager esp_event driver board_manager
)
