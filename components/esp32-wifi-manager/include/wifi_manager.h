/*
 *  File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 *  Created on: 2025-06-13 10:00:00
 *  Edited on:  2025-06-15 10:18:00 (CDT)
 *
 *  Author: R. Andrew Ballard (c) 2025
 *  Version: v8.3.0
 *
 *  Description:
 *    Wi-Fi Manager interface for PianoGuard. Handles NVS-based credential load,
 *    automatic fallback to AP provisioning, and optional boot-time reset via GPIO.
 */

#pragma once

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_CONNECTED_BIT (1 << 0)

/**
 * @brief Callback type for notifying connection success
 */
typedef void (*wifi_connected_cb_t)(void);

/**
 * @brief Initialize Wi-Fi manager (must be called before start).
 */
void wifi_manager_init(void);

/**
 * @brief Start Wi-Fi logic: STA if credentials found, AP fallback otherwise.
 *        If GPIO reset pin is held low at boot, clears credentials.
 *
 * @param cb Optional callback invoked after successful STA connection.
 */
void wifi_manager_start(wifi_connected_cb_t cb);

/**
 * @brief Get internal event group handle for connection sync.
 *
 * @return EventGroupHandle_t used by Wi-Fi manager.
 */
EventGroupHandle_t wifi_event_group_handle(void);

#endif // WIFI_MANAGER_H
