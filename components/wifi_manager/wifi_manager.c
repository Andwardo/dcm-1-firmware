/*
 *  wifi_manager.h
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-08
 *      Author: Andwardo
 *      Version: v8.2.46
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/event_groups.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_MANAGER_STA_CONNECTED_BIT BIT0

void wifi_manager_start(void);
EventGroupHandle_t wifi_manager_get_event_group(void);
void generate_ap_ssid_from_mac(void);
const char* get_ap_ssid(void);

#ifdef __cplusplus
}
#endif

#endif // WIFI_MANAGER_H