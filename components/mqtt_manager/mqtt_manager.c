/**
 * File: mqtt_manager.h
 * Description: MQTT client interface with certificate loading and callbacks.
 * Created on: 2025-06-15
 * Edited on:  2025-06-30
 * Version: v8.5.6
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "mqtt_client.h"

typedef void (*mqtt_data_callback_t)(esp_mqtt_event_handle_t event);
typedef void (*mqtt_error_callback_t)(esp_mqtt_error_codes_t *error_handle);

void start_mqtt_manager(void);
void mqtt_manager_publish(const char* topic, const char* data);
void mqtt_manager_register_data_callback(mqtt_data_callback_t callback);
void mqtt_manager_register_error_callback(mqtt_error_callback_t callback);

#endif // MQTT_MANAGER_H
