/*
 *  wifi_manager.c
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-08
 *      Author: Andwardo
 *      Version: v8.2.46
 */

#include "wifi_manager.h"
#include "http_app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include <string.h>

#define AP_SSID_PREFIX "PianoGuardSetup-"
#define DEFAULT_AP_PASSWORD "pianoguard"
#define DEFAULT_AP_CHANNEL 1
#define DEFAULT_AP_MAX_CONNECTIONS 4
#define DEFAULT_AP_HIDE_SSID 0
#define DEFAULT_AP_BEACON_INTERVAL 100

static char dynamic_ap_ssid[33]; // Max SSID length = 32 + null terminator

void generate_ap_ssid_from_mac() {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);  // SoftAP MAC
    snprintf(dynamic_ap_ssid, sizeof(dynamic_ap_ssid), AP_SSID_PREFIX"%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

const char* get_ap_ssid() {
    return dynamic_ap_ssid;
}

static const char *TAG = "wifi_manager";

static void wifi_manager_task(void *pvParameters) {
    ESP_LOGI(TAG, "Starting Wi-Fi manager task");

    generate_ap_ssid_from_mac();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    wifi_config_t ap_config = {
        .ap = {
            .channel = DEFAULT_AP_CHANNEL,
            .max_connection = DEFAULT_AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = DEFAULT_AP_HIDE_SSID,
            .beacon_interval = DEFAULT_AP_BEACON_INTERVAL,
        },
    };

    strncpy((char *)ap_config.ap.ssid, dynamic_ap_ssid, sizeof(ap_config.ap.ssid));
    ap_config.ap.ssid_len = strlen(dynamic_ap_ssid);
    strncpy((char *)ap_config.ap.password, DEFAULT_AP_PASSWORD, sizeof(ap_config.ap.password));

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    http_app_start();

    vTaskDelete(NULL);
}

void wifi_manager_start() {
    xTaskCreate(wifi_manager_task, "wifi_manager_task", 4096, NULL, 5, NULL);
}