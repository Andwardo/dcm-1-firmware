/*
 * File: components/mqtt_manager/include/mqtt_manager.h
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 10:00:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <freertos/event_groups.h>
#include "esp_err.h"
#include <stdbool.h>

/** Bit set in event group when MQTT connects. */
#define MQTT_CONNECTED_BIT (1 << 1)

/** Initialize MQTT manager (creates client, event group). */
void mqtt_manager_init(void);

/** Start MQTT client (non-blocking). */
void mqtt_manager_start(void);

/** Retrieve internal MQTT event group handle for waiting. */
EventGroupHandle_t mqtt_event_group_handle(void);

/**
 * Publish the current sensor state as JSON to the broker.
 * @return ESP_OK on success, error otherwise.
 */
esp_err_t mqtt_manager_publish_sensor_state(bool power, bool water, bool pads);

#endif // MQTT_MANAGER_H
