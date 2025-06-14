/*
 * File: components/provisioning_manager/include/provisioning_manager.h
 *
 * Created on: 14 June 2025 10:00:00
 * Last edited on: 14 June 2025 10:00:00
 *
 * Version: 1.0
 */

#ifndef PROVISIONING_MANAGER_H
#define PROVISIONING_MANAGER_H

#include "esp_err.h"

/**
 * @brief Initialize the provisioning workflow (HTTP/Scheme).
 *        Launches AP and web server to collect SSID/password.
 */
esp_err_t provisioning_manager_init(void);

/**
 * @brief Retrieve stored Wi-Fi credentials after provisioning.
 *
 * @param[out] ssid      Buffer to receive SSID (max 32 chars + null).
 * @param[out] password  Buffer to receive password (max 64 chars + null).
 * @return ESP_OK on success, error otherwise.
 */
esp_err_t provisioning_manager_get_credentials(char *ssid, char *password);

#endif // PROVISIONING_MANAGER_H
