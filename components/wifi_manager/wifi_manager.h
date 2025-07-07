/*
 *  wifi_manager.h
 *
 *  Created on: 2025-07-07
 *  Edited on: 2025-07-07 (local time)
 *      Author: Andwardo
 *      Version: v8.3.0
 */

#ifndef WIFI_MANAGER_H_
#define WIFI_MANAGER_H_

#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"

/* Event group bits */
#define WIFI_MANAGER_WIFI_CONNECTED_BIT      BIT0
#define WIFI_MANAGER_WIFI_DISCONNECTED_BIT   BIT1
#define WIFI_MANAGER_AP_STARTED_BIT          BIT2
#define WIFI_MANAGER_AP_STOPPED_BIT          BIT3

/* WiFi Manager API */
void wifi_manager_start(void);
void wifi_manager_stop(void);
bool wifi_manager_is_connected(void);
void wifi_manager_reset_settings(void);

/* Event group handle exposed for external sync (optional) */
extern EventGroupHandle_t wifi_event_group;

/* Network interface getter */
esp_netif_t *wifi_manager_get_netif_sta(void);
esp_netif_t *wifi_manager_get_netif_ap(void);

#endif /* WIFI_MANAGER_H_ */