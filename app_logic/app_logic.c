/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 11 June 2025 21:20:00
 * Last edited on: 12 June 2025 09:10:00
 *
 * Version: 7.5.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "app_logic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "board_manager.h"
#include "wifi_manager.h" 
#include "mqtt_manager.h" // Include the new manager

static const char *TAG = "APP_LOGIC";

static void app_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    xEventGroupWaitBits(wifi_manager_event_group, WIFI_MANAGER_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi is connected. Starting MQTT Manager.");
    mqtt_manager_start();

    ESP_LOGI(TAG, "Main application loop running.");

    while(1) {
        // Main loop logic will go here
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_logic_start_task(void)
{
    xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}

