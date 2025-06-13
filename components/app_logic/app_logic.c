/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 10:00:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "app_logic.h"
#include "board_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "esp_log.h"
#include "esp_system.h"
#include <freertos/event_groups.h>

static const char *TAG = "APP_LOGIC";

void app_logic_task(void *param)
{
    // Allow pins to stabilize
    vTaskDelay(pdMS_TO_TICKS(100));

    // Initialize board peripherals
    esp_err_t err = board_manager_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "board_manager_init failed: %s", esp_err_to_name(err));
        vTaskDelete(NULL);
        return;
    }

    // Check factory reset button
    if (board_manager_is_reset_button_pressed()) {
        ESP_LOGW(TAG, "Factory reset triggered");
        board_manager_do_factory_reset();
        esp_restart();
    }

    ESP_LOGI(TAG, "Board OK — initializing Wi-Fi");

    // Phase 2: Wi-Fi
    wifi_manager_init();
    wifi_manager_start(NULL);

    // Wait indefinitely for Wi-Fi connection
    xEventGroupWaitBits(
        wifi_event_group_handle(),
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY
    );
    ESP_LOGI(TAG, "Wi-Fi connected — starting MQTT");

    // Phase 3: MQTT
    mqtt_manager_init();
    mqtt_manager_start();

    // Wait up to 10s for MQTT connection
    xEventGroupWaitBits(
        mqtt_event_group_handle(),
        MQTT_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        pdMS_TO_TICKS(10000)
    );
    ESP_LOGI(TAG, "MQTT connected — entering sensor loop");

    // Sensor read and publish loop
    while (true) {
        bool power, water, pads;
        board_manager_read_led_states(&power, &water, &pads);
        mqtt_manager_publish_sensor_state(power, water, pads);
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
