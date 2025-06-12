/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 * Created on: 12 June 2025 07:20:00
 * Last edited on: 12 June 2025 21:30:00
 *
 * Version: 7.4.5
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include "esp_err.h"

/**
 * @brief Defines the callback type for a successful connection.
 */
typedef void (*wifi_connected_event_callback_t)(void);

/**
 * @brief Starts the Wi-Fi manager. This is non-blocking.
 * It will attempt to connect using saved credentials, or start provisioning
 * mode if none are found.
 * @param cb The function to call after a successful Wi-Fi connection.
 */
void wifi_manager_start(wifi_connected_event_callback_t cb);

/**
 * @brief Saves new Wi-Fi credentials and restarts the device.
 * @param ssid The SSID of the network to connect to.
 * @param password The password for the network.
 */
void wifi_manager_save_credentials_and_restart(const char* ssid, const char* password);

#endif /* WIFI_MANAGER_H_ */
