/*
 *  wifi_manager.h
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-08
 *      Author: Andwardo
 *      Version: v8.2.45
 */

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

#define WIFI_MANAGER_STA_CONNECTED_BIT BIT0

EventGroupHandle_t wifi_manager_get_event_group(void);

#endif // WIFI_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts the Wi-Fi manager task.
 *
 * This function initializes the Wi-Fi stack, sets up the AP/STA mode,
 * and launches the HTTP web server.
 */
void wifi_manager_start(void);

/**
 * @brief Generates the dynamic SSID string from the SoftAP MAC address.
 *
 * Should be called before accessing get_ap_ssid().
 */
void generate_ap_ssid_from_mac(void);

/**
 * @brief Returns a pointer to the dynamically generated SoftAP SSID.
 *
 * @return const char* pointer to SSID string
 */
const char* get_ap_ssid(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_ */