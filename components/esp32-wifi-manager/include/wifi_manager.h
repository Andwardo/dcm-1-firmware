/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 10:00:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <freertos/event_groups.h>

/** Bit set in event group when Wi-Fi connects. */
#define WIFI_CONNECTED_BIT (1 << 0)

/** Prototype for optional connect callback. */
typedef void (*wifi_connected_cb_t)(void);

/** Initialize Wi-Fi manager (must be called before start). */
void wifi_manager_init(void);

/**
 * Start Wi-Fi (non-blocking).
 * @param cb Optional callback invoked when connected (can be NULL).
 */
void wifi_manager_start(wifi_connected_cb_t cb);

/**
 * Retrieve internal Wi-Fi event group handle for waiting.
 */
EventGroupHandle_t wifi_event_group_handle(void);

#endif // WIFI_MANAGER_H
