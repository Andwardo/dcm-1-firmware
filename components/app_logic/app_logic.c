/*
 * File:    components/app_logic/app_logic.c
 * Description: Main application logic task. Reads sensor data and prepares it for publishing.
 * Created on: 2025-06-11
 * Edited on:  2025-07-03
 * Version: v8.3.2
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
#include "board_manager.h" // Include our new board manager

static const char *TAG = "APP_LOGIC";

static void app_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    // 1. Block until the Wi-Fi STA is up
    EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
    xEventGroupWaitBits(wifi_events,
                        WIFI_MANAGER_STA_CONNECTED_BIT,
                        pdFALSE,  // don't clear bit
                        pdFALSE,  // wait for any one bit
                        portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi is connected. Entering main loop.");

    dcm_status_t current_status;
    char         json_payload[200];

    while (1) {
        // 2. Read board status
        if (board_manager_get_status(&current_status) == ESP_OK) {
            // 3. Build JSON
            cJSON *root = cJSON_CreateObject();
            if (root) {
                cJSON_AddBoolToObject(root, "power", current_status.power_ok);
                cJSON_AddBoolToObject(root, "water", !current_status.water_low);
                cJSON_AddBoolToObject(root, "pads",  !current_status.pads_worn);

                if (cJSON_PrintPreallocated(root,
                                            json_payload,
                                            sizeof(json_payload),
                                            false)) {
                    ESP_LOGI(TAG, "Status Payload: %s", json_payload);
                    // mqtt_manager_publish("dcm/status/DEVICE_ID", json_payload);
                }
                cJSON_Delete(root);
            }
        } else {
            ESP_LOGE(TAG, "Failed to read board status");
        }

        // 4. Delay until next cycle
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_logic_run(void)
{
    xTaskCreate(app_task,
                "app_logic_task",
                4096,
                NULL,
                5,
                NULL);
}
