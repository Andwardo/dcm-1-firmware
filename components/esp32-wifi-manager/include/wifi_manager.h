/*
 * File: components/esp32-wifi-manager/include/wifi_manager.h
 * Version: v8.2.7
 * Author: R. Andrew Ballard (c) 2025
 */
#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi_types.h"

#define WIFI_MANAGER_AP_STARTED_BIT      BIT0
#define WIFI_MANAGER_STA_CONNECTED_BIT   BIT1
#define WIFI_MANAGER_STA_DISCONNECTED_BIT BIT2

typedef enum {
    WIFI_MANAGER_MSG_START_PROVISIONING,
    WIFI_MANAGER_MSG_CONNECT_STA,
} wifi_manager_message_id_e;

typedef struct {
    wifi_manager_message_id_e msg_id;
    wifi_config_t sta_config;
} wifi_manager_message_t;

void wifi_manager_init(void);
BaseType_t wifi_manager_send_message(wifi_manager_message_id_e msg_id, const void *data);
EventGroupHandle_t wifi_manager_get_event_group(void);

#endif
