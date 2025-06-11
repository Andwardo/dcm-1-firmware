/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 * This is the header file for the esp32-wifi-manager library by Tony Pottier.
 *
 * Version: 7.1.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include "esp_err.h"
#include "esp_wifi_types.h"

/**
 * @brief Starts the Wi-Fi manager.
 *
 * This function initializes and starts the Wi-Fi manager. It handles:
 * - Connecting to a previously saved Wi-Fi network.
 * - If no network is saved, or connection fails, it starts a web server
 * and an Access Point for configuration.
 *
 * The device will create an AP with the SSID "PianoGuard_DCM-1". Connect to it
 * and a captive portal should appear to configure Wi-Fi.
 */
void wifi_manager_start();

#endif /* WIFI_MANAGER_H_ */

