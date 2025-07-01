/**
 * File: mqtt_manager.h
 * Description: Header for managing the secure MQTT connection using mTLS.
 * Created on: 2025-06-15
 * Edited on:  2025-07-01
 * Version: v8.6.7
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "mqtt_client.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback type for handling incoming MQTT data events.
 */
typedef void (*mqtt_data_callback_t)(esp_mqtt_event_handle_t event);

/**
 * @brief Callback type for handling MQTT error events.
 */
typedef void (*mqtt_error_callback_t)(esp_mqtt_error_codes_t *error_handle);

/**
 * @brief Initializes the MQTT manager, loads certs, and connects using mTLS.
 *
 * Must be called once after Wi-Fi is connected.
 */
void mqtt_manager_init(void);

/**
 * @brief Publishes a message to a given MQTT topic.
 *
 * @param topic The topic to publish to (e.g., "piano/status").
 * @param data  The message payload (e.g., JSON string or raw data).
 */
void mqtt_manager_publish(const char* topic, const char* data);

/**
 * @brief Registers a callback to handle MQTT data events.
 *
 * @param callback Function to be called when MQTT_EVENT_DATA is received.
 */
void mqtt_manager_register_data_callback(mqtt_data_callback_t callback);

/**
 * @brief Registers a callback to handle MQTT error events.
 *
 * @param callback Function to be called when MQTT_EVENT_ERROR occurs.
 */
void mqtt_manager_register_error_callback(mqtt_error_callback_t callback);

#ifdef __cplusplus
}
#endif

#endif // MQTT_MANAGER_H
