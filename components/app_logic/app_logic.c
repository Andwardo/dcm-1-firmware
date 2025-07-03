/*
 * File: components/app_logic/app_logic.c
 * Description: Main application logic task. Reads sensor data and prepares it for publishing.
 *
 * Created on: 2025-06-11
 * Edited on:  2025-07-03
 *
 * Version: v8.3.2
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#include "app_logic.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "cJSON.h"

// Project Components
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "board_manager.h"  // Include our new board manager

static const char *TAG = "APP_LOGIC";

static void app_task(void *pvParameter) {
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    // Wait for the wifi_manager to signal that we are connected
    EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
    xEventGroupWaitBits(wifi_events, WIFI_MANAGER_STA_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi is connected. Starting main application loop.");
    // In the next and final phase, we will start the mqtt_manager here.
    // mqtt_manager_start();

    dcm_status_t current_status;
    char json_payload[200];

    while (1) {
        // 1. Get status from the real hardware via the board manager
        if (board_manager_get_status(&current_status) == ESP_OK) {
            
            // 2. Build the JSON payload with real data
            cJSON *root = cJSON_CreateObject();
            if (root) {
                // The JSON fields here match the logic from your product pitch.
                // "power" is true if power is OK.
                // "water" is true if water is NOT low.
                // "pads" is true if pads are NOT worn.
                cJSON_AddBoolToObject(root, "power", current_status.power_ok);
                cJSON_AddBoolToObject(root, "water", !current_status.water_low);
                cJSON_AddBoolToObject(root, "pads", !current_status.pads_worn);
                
                if (cJSON_PrintPreallocated(root, json_payload, sizeof(json_payload), false)) {
                    ESP_LOGI(TAG, "Current Status Payload: %s", json_payload);
                    // 3. In the final phase, we will publish this payload via MQTT
                    // mqtt_manager_publish("dcm/status/your_device_id", json_payload);
                }
                cJSON_Delete(root);
            }
        } else {
            ESP_LOGE(TAG, "Failed to get board status");
        }
        
        // Wait for the next cycle
        vTaskDelay(pdMS_TO_TICKS(5000)); // Read sensors every 5 seconds
    }
}

void app_logic_run(void) {
    xTaskCreate(app_task, "app_logic_task", 4096, NULL, 5, NULL);
}
