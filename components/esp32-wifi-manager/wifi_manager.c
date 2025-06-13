/*
 * File: components/esp32-wifi-manager/wifi_manager.c
 *
 * Created on: 12 June 2025 22:10:00
 * Last edited on: 12 June 2025 23:10:00
 *
 * Version: 7.9.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
#include "wifi_manager.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "http_app.h"

EventGroupHandle_t wifi_manager_event_group;
const int WIFI_MANAGER_CONNECTED_BIT = BIT0;

static const char TAG[] = "WIFI_MANAGER";
static bool is_connected = false;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(is_connected){
            is_connected = false;
            ESP_LOGE(TAG, "Disconnected from AP. Retrying...");
        }
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if(!is_connected){
            is_connected = true;
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
            xEventGroupSetBits(wifi_manager_event_group, WIFI_MANAGER_CONNECTED_BIT);
        }
    }
}

void wifi_manager_init(void)
{
    wifi_manager_event_group = xEventGroupCreate();
}

void wifi_manager_start() {
    // ... (rest of function is unchanged)
}
void wifi_manager_save_credentials_and_restart(const char* ssid, const char* password) {
    // ... (rest of function is unchanged)
}
