/*
 *  mqtt_manager.h
 *
 *  Created on: 2025-06-15
 *  Edited on: 2025-06-15 (CDT)
 *      Author: Andwardo
 *      Version: v1.1
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "esp_event.h"

/**
 * @brief Initialize the MQTT manager.
 */
void mqtt_manager_init(void);

/**
 * @brief Start the MQTT manager (connect + subscribe).
 */
void mqtt_manager_start(void);

/**
 * @brief Publish a payload to a topic.
 *
 * @param topic   Null-terminated MQTT topic string.
 * @param payload Null-terminated JSON or text payload.
 */
void mqtt_manager_publish(const char *topic, const char *payload);

#ifdef __cplusplus
}
#endif
