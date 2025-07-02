/**
 * File: wifi_manager.h
 * Description: Wi-Fi manager header for PianoGuard DCM-1
 * Created on: 2025-06-18
 * Edited on:  2025-07-01
 * Version: v8.6.8
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Message types used by the Wi-Fi Manager task
typedef enum {
    WIFI_MANAGER_MSG_CONNECT_STA,
    WIFI_MANAGER_MSG_START_PROVISIONING
} wifi_manager_message_e;

// Message struct passed to the Wi-Fi Manager task
typedef struct {
    int msg_id;
    wifi_config_t sta_config;
} wifi_manager_message_t;

// Function declarations
void wifi_manager_init(void);
BaseType_t wifi_manager_send_message(void *msg);
EventGroupHandle_t wifi_manager_get_event_group(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
