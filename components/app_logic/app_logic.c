/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 12 June 2025 21:20:00
 * Last edited on: 12 June 2025 21:40:00
 *
 * Version: 7.6.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "app_logic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "board_manager.h"
// We will add wifi and mqtt back in the next step
// #include "wifi_manager.h"
// #include "mqtt_manager.h"

static const char *TAG = "APP_LOGIC";

static void app_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started.");

    // Initialize board hardware
    ESP_ERROR_CHECK(board_manager_init());

    // Allow GPIOs to stabilize
    vTaskDelay(pdMS_TO_TICKS(100)); 

    // Check for factory reset condition
    if (board_manager_is_reset_button_pressed()) {
        ESP_LOGW(TAG, "Factory reset detected. Wiping NVS and restarting...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        esp_restart();
    }

    ESP_LOGI(TAG, "No factory reset detected. Proceeding with normal boot.");
    ESP_LOGI(TAG, "Main application loop running.");

    while(1) {
        gpio_set_level(STATUS_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(STATUS_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void app_logic_start_task(void)
{
    xTaskCreate(app_task, "app_task", 4096, NULL, 5, NULL);
}
