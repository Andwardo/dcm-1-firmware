/**
 * File: mqtt_manager.h
 * Description: Public interface for the MQTT Connection Manager.
 * Created on: 2025-06-15
 * Edited on: 2025-06-24
 * Version: v8.3.0
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

/**
 * @brief Initializes and starts the MQTT client, connecting to the broker
 * using the secure certificates loaded from flash.
 */
void start_mqtt_manager(void);

#endif // MQTT_MANAGER_H
