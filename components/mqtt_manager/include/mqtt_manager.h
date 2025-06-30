/**
 * File: mqtt_manager.h
 * Description: Header for managing the secure MQTT connection using mTLS.
 * Created on: 2025-06-15
 * Edited on:  2025-06-30
 * Version: v8.5.5
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts the MQTT manager by initializing and connecting the MQTT client
 *        using mTLS certificates loaded from flash.
 */
void start_mqtt_manager(void);

/**
 * @brief Publishes a message to a given MQTT topic.
 *
 * @param topic The topic to publish to (e.g., "piano/status")
 * @param data  The message payload (e.g., JSON string or raw data)
 */
void mqtt_manager_publish(const char* topic, const char* data);

#ifdef __cplusplus
}
#endif

#endif // MQTT_MANAGER_H
