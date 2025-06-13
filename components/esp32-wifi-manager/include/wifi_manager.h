/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 * Created on: 12 June 2025 22:10:00
 * Last edited on: 12 June 2025 23:10:00
 *
 * Version: 7.9.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include "freertos/event_groups.h"

/* Event group to signal when Wi-Fi is connected */
extern EventGroupHandle_t wifi_manager_event_group;
extern const int WIFI_MANAGER_CONNECTED_BIT;

/**
 * @brief Initializes the Wi-Fi manager's internal resources (e.g., event group).
 * Must be called before wifi_manager_start().
 */
void wifi_manager_init(void);

/**
 * @brief Starts the Wi-Fi connection process. This is non-blocking.
 */
void wifi_manager_start(void);

/**
 * @brief Saves new Wi-Fi credentials and restarts the device.
 */
void wifi_manager_save_credentials_and_restart(const char* ssid, const char* password);

#endif /* WIFI_MANAGER_H_ */
