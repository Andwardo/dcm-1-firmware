/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 * Created on: 2025-06-15
 * Edited on:  2025-06-16
 *
 * Version: v8.0.7
 *
 * Author: R. Andrew Ballard (c)2025
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>
#include "freertos/event_groups.h" // Add this for EventGroupHandle_t

/* Bit definitions for the Wi-Fi event group */
#define WIFI_CONNECTED_BIT BIT0 // The bit to set when connected

/* Function Declarations */
esp_err_t wifi_manager_init(void);
bool      wifi_manager_has_saved_credentials(void);
void      wifi_manager_start_provisioning(void);
esp_err_t wifi_manager_connect(void);
bool      wifi_manager_is_ready(void);
bool      wifi_manager_is_sta_connected(void);

/**
 * @brief Gets the handle to the Wi-Fi event group.
 *
 * @return The EventGroupHandle_t for the Wi-Fi event group.
 */
EventGroupHandle_t wifi_event_group_handle(void); // Add this getter function

#endif // WIFI_MANAGER_H
