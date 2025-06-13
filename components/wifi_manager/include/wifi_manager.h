/*
 * File: components/wifi_manager/include/wifi_manager.h
 *
 * Created on: 13 June 2025 09:25:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.0.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

/**
 * @brief Starts the Wi-Fi connection process.
 */
void wifi_manager_start(void);

/**
 * @brief Waits for the Wi-Fi connection to be established.
 *
 * @param timeout The maximum ticks to wait. Use portMAX_DELAY for infinite.
 * @return true if connected, false on timeout.
 */
bool wifi_manager_wait_for_connect(TickType_t timeout);

#endif // WIFI_MANAGER_H
