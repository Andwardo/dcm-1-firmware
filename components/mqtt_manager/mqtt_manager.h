/*
 * File: components/mqtt_manager/include/mqtt_manager.h
 *
 * Created on: 13 June 2025 09:35:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.0.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "esp_err.h"

/**
 * @brief Starts the MQTT client and connects to the broker.
 *
 * Must be called after Wi-Fi is connected.
 *
 * @return esp_err_t ESP_OK on success, error otherwise.
 */
esp_err_t mqtt_manager_start(void);

/**
 * @brief Publishes a message to a specific MQTT topic.
 *
 * @param topic   The MQTT topic.
 * @param payload The message payload string.
 * @return int The message ID on success, -1 on failure.
 */
int mqtt_manager_publish(const char *topic, const char *payload);

#endif // MQTT_MANAGER_H
