/*
 *  wifi_manager.h
 *
 *  Description: Public interface for the PianoGuard Wi-Fi Manager.
 *               Manages provisioning, STA/AP connections, and message-based task control.
 *
 *  Created on:  2017–2020 (Original by Tony Pottier)
 *  Edited on:   2025-07-01
 *  Version:     v1.6
 *  Author:      Andwardo
 */

#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Message Queue Definitions ---

typedef enum {
    WIFI_MANAGER_MSG_START_PROVISIONING = 0,
    WIFI_MANAGER_MSG_CONNECT_STA
} wifi_manager_msg_id_t;

typedef struct {
    wifi_manager_msg_id_t msg_id;
    wifi_config_t sta_config;  // Used only with CONNECT_STA
} wifi_manager_message_t;

// --- Event Group Bits ---

#define WIFI_MANAGER_STA_CONNECTED_BIT     BIT0
#define WIFI_MANAGER_STA_DISCONNECTED_BIT  BIT1
#define WIFI_MANAGER_AP_STARTED_BIT        BIT2

// --- Public API ---

/**
 * @brief Initializes the Wi-Fi Manager and launches its task.
 */
void wifi_manager_init(void);

/**
 * @brief Sends a message to the internal Wi-Fi manager task queue.
 */
BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg);

/**
 * @brief Returns the Wi-Fi manager's event group handle.
 */
EventGroupHandle_t wifi_manager_get_event_group(void);

/**
 * @brief Begins SoftAP mode and prepares for provisioning.
 */
void wifi_manager_start_provisioning(void);

/**
 * @brief Sends credentials asynchronously to the Wi-Fi manager task.
 */
void wifi_manager_connect_async(const char *ssid, const char *password);

/**
 * @brief Returns the STA netif handle used for DNS server and other bindings.
 */
esp_netif_t* wifi_manager_get_esp_netif_sta(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
