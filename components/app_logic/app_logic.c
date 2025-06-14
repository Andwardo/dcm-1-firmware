/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 13 June 2025 09:20:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.1.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "app_logic.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "board_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_LOGIC";
static TaskHandle_t app_logic_task_handle = NULL;

static void handle_factory_reset(void) {
    ESP_LOGW(TAG, "Factory reset triggered!");
    ESP_ERROR_CHECK(nvs_flash_erase());
    esp_restart();
}

static void app_logic_task(void *pvParameters) {
    ESP_LOGI(TAG, "Application logic task started.");

    // Phase 1: Stabilize Architecture
    vTaskDelay(pdMS_TO_TICKS(200));
    ESP_ERROR_CHECK(board_manager_init());
    if (board_manager_get_factory_reset_button_state()) {
        handle_factory_reset();
    }
    ESP_LOGI(TAG, "Architecture stable. Proceeding to networking.");

    // Phase 2: Networking
    wifi_manager_start();
    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");
    if (!wifi_manager_wait_for_connect(portMAX_DELAY)) {
        ESP_LOGE(TAG, "Wi-Fi connect failed; halting.");
        while (1) { vTaskDelay(portMAX_DELAY); }
    }
    ESP_LOGI(TAG, "Wi-Fi connected.");

    // Phase 3: MQTT
    ESP_LOGI(TAG, "Starting MQTT client.");
    ESP_ERROR_CHECK(mqtt_manager_start());

    // Main loop
    bool led = false;
    while (1) {
        led = !led;
        board_manager_set_led_state(led);
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void app_logic_start(void) {
    xTaskCreate(app_logic_task, "app_logic_task", 4096, NULL, 5, &app_logic_task_handle);
}
