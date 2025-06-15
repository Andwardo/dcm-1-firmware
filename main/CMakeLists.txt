#
# File: main/CMakeLists.txt
#
# Created on: 12 June 2025 23:30:00
# Last edited on: 12 June 2025 23:30:00
#
# Version: 8.0.0
#
# Author: R. Andrew Ballard (c) 2025
#
idf_component_register(SRCS "main.c"
                INCLUDE_DIRS "."
                PRIV_REQUIRES 
		    esp32-wifi-manager 
		    mqtt_manager 
		    esp_event
		    nvs_flash
		    app_logic
		    board_manager
)
