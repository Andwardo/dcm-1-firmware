/*
 *  wifi_manager.h
 *
 *  Created on: 2017–2020 (Original by Tony Pottier)
 *  Edited on: 2025-06-30
 *      Author: Andwardo
 *      Version: v1.4
 */

#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message ID for internal queue
typedef enum {
    WIFI_MANAGER_MSG_START_PROVISIONING = 0,
    WIFI_MANAGER_MSG_CONNECT_STA
} wifi_manager_msg_id_t;

// Struct passed through message queue
typedef struct {
    wifi_manager_msg_id_t msg_id;
    wifi_config_t sta_config;  // Used only for CONNECT_STA
} wifi_manager_message_t;

// Event bits
#define WIFI_MANAGER_STA_CONNECTED_BIT     BIT0
#define WIFI_MANAGER_STA_DISCONNECTED_BIT  BIT1
#define WIFI_MANAGER_AP_STARTED_BIT        BIT2

// API
void wifi_manager_init(void);
BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg);
EventGroupHandle_t wifi_manager_get_event_group(void);
void wifi_manager_start_provisioning(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
