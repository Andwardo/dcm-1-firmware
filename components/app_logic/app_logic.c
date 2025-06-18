/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 2025-06-11
 * Edited on:  2025-06-18
 *
 * Version: v8.2.6
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#include "app_logic.h"
#include <stdio.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"

// Project Components
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_LOGIC";

static void app_task(void *pvParameter) {
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    // Use the new function name and event bit define from the wifi_manager
    EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
    xEventGroupWaitBits(wifi_events, WIFI_MANAGER_STA_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi is connected. Starting main application loop.");
    // We will start MQTT and other services here in a later phase.
    // mqtt_manager_start();

    while (1) {
        ESP_LOGI(TAG, "Application loop running...");
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_logic_init(void) {
    xTaskCreate(app_task, "app_logic_task", 4096, NULL, 5, NULL);
}
