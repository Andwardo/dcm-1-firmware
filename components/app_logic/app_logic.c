/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 12 June 2025 23:30:00
 * Last edited on: 12 June 2025 23:30:00
 *
 * Version: 8.0.0
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
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_LOGIC";

static void on_wifi_connected(void)
{
    ESP_LOGI(TAG, "Wi-Fi connection successful. Starting MQTT Manager.");
    mqtt_manager_start();
}

static void app_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started.");
    ESP_ERROR_CHECK(board_manager_init());
    vTaskDelay(pdMS_TO_TICKS(100));
    if (board_manager_is_reset_button_pressed()) {
        ESP_LOGW(TAG, "Factory reset detected. Wiping NVS and restarting...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        esp_restart();
    }
    ESP_LOGI(TAG, "No factory reset detected. Starting Wi-Fi Manager.");
    wifi_manager_start(on_wifi_connected);

    while(1) {
        gpio_set_level(STATUS_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(200));
        gpio_set_level(STATUS_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(1800));
    }
}

void app_logic_start_task(void)
{
    xTaskCreate(app_task, "app_task", 8192, NULL, 5, NULL);
}
