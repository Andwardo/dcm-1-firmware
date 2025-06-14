/*
 * File: components/wifi_manager/include/wifi_manager.h
 *
 * Created on: 13 June 2025 11:00:00
 * Last edited on: 14 June 2025 10:15:00
 *
 * Version: 8.0.3
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"

/**
 * @brief Start the Wi-Fi manager. Will auto-provision if no credentials.
 */
esp_err_t wifi_manager_start(void);

/**
 * @brief Stop Wi-Fi and cleanup.
 */
esp_err_t wifi_manager_stop(void);

#endif // WIFI_MANAGER_H
