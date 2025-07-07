/*
 *  wifi_manager.c
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-07 (local time)
 *      Author: Andwardo
 *      Version: v8.2.43
 */
#include <string.h>
#include "wifi_manager.h"
#include "http_app.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

static const char *TAG = "wifi_manager";

static void wifi_manager_task(void *pvParameters) {
    ESP_LOGI(TAG, "Starting Wi-Fi manager task");

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = DEFAULT_AP_SSID,
            .ssid_len = strlen(DEFAULT_AP_SSID),
            .password = DEFAULT_AP_PASSWORD,
            .channel = DEFAULT_AP_CHANNEL,
            .max_connection = DEFAULT_AP_MAX_CONNECTIONS,
            .authmode = WIFI_AUTH_WPA2_PSK,
            .ssid_hidden = DEFAULT_AP_HIDE_SSID,
            .beacon_interval = DEFAULT_AP_BEACON_INTERVAL,
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    http_app_start();

    vTaskDelete(NULL);
}

void wifi_manager_start() {
    xTaskCreate(wifi_manager_task, "wifi_manager_task", 4096, NULL, 5, NULL);
}