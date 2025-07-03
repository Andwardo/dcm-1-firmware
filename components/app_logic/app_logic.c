/*
 * File:    components/app_logic/app_logic.c
 * Description: Main application logic task. Reads sensor data and prepares it for publishing.
 * Created on: 2025-06-11
 * Edited on:  2025-07-08
 * Version: v8.3.3
 * Author:  R. Andrew Ballard (c) 2025
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
#include "board_manager.h"

static const char *TAG = "APP_LOGIC";

// internal worker function: blocks until Wi-Fi connected, then runs the main loop
static void app_logic_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    // 1) wait for station bit to be set
    EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
    xEventGroupWaitBits(wifi_events,
                        WIFI_MANAGER_STA_CONNECTED_BIT,
                        pdFALSE,
                        pdFALSE,
                        portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi connected. Entering status loop.");

    dcm_status_t current_status;
    char json_payload[200];

    while (1) {
        if (board_manager_get_status(&current_status) == ESP_OK) {
            cJSON *root = cJSON_CreateObject();
            if (root) {
                cJSON_AddBoolToObject(root,  "power", current_status.power_ok);
                cJSON_AddBoolToObject(root,  "water", !current_status.water_low);
                cJSON_AddBoolToObject(root,  "pads",  !current_status.pads_worn);

                if (cJSON_PrintPreallocated(root, json_payload, sizeof(json_payload), false)) {
                    ESP_LOGI(TAG, "Status Payload: %s", json_payload);
                    // mqtt_manager_publish("dcm/status/DEVICE_ID", json_payload);
                }
                cJSON_Delete(root);
            }
        } else {
            ESP_LOGE(TAG, "Failed to read board status");
        }
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

/**
 * @brief Create the FreeRTOS task that runs the application logic.
 */
void app_logic_init(void)
{
    xTaskCreate(app_logic_task,
                "app_logic_task",
                4096,
                NULL,
                5,
                NULL);
}

/**
 * @brief Run the application logic directly in this context (blocking).
 *
 * Useful if you don't want a separate FreeRTOS task.
 */
void app_logic_run(void)
{
    // call the same worker function directly
    app_logic_task(NULL);
}
