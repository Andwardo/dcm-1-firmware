/*
 *  wifi_manager.h
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-07 (local time)
 *      Author: Andwardo
 *      Version: v8.2.42
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_AP_SSID           "ESP32_AP"
#define DEFAULT_AP_PASSWORD       "12345678"
#define DEFAULT_AP_CHANNEL        1
#define DEFAULT_AP_MAX_CONNECTIONS 4
#define DEFAULT_AP_HIDE_SSID      0
#define DEFAULT_AP_BEACON_INTERVAL 100

void wifi_manager_start(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H