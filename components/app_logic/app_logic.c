/*
 * File: components/app_logic/app_logic.c
 *
 * Version: v8.0.8
 * Author: R. Andrew Ballard (c) 2025
 */

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

/* C Standard Library */
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* FreeRTOS (MUST be first) */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* Project Components */
#include "app_logic.h"
#include "board_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

/* ESP-IDF */
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "APP_LOGIC";

typedef struct {
    bool power;
    bool water;
    bool pads;
} dcm_status_t;

static dcm_status_t read_dcm_inputs_stub(void) {
    static bool toggle = false;
    toggle = !toggle;
    dcm_status_t state = {.power = true, .water = toggle, .pads  = !toggle};
    return state;
}

static void app_task(void *pvParameter) {
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");
    xEventGroupWaitBits(wifi_event_group_handle(), WIFI_CONNECTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    ESP_LOGI(TAG, "Wi-Fi is connected. Starting MQTT Manager...");
    mqtt_manager_start();

    ESP_LOGI(TAG, "Main application loop running.");
    while (1) {
        dcm_status_t status = read_dcm_inputs_stub();
        time_t now = time(NULL);
        char payload[256];

        /*
         * UPDATED: The format string now uses "%s" for the device_id
         * to correctly match the five arguments being passed to snprintf.
         */
        snprintf(payload, sizeof(payload),
                 "{\"device_id\":\"%s\",\"timestamp\":%lld,\"status\":{"
                 "\"power\":%s,\"water\":%s,\"pads\":%s}}",
                 "DCM-0001", (long long)now,
                 status.power ? "true" : "false",
                 status.water ? "true" : "false",
                 status.pads  ? "true" : "false"
        );

        mqtt_manager_publish("dcm/status/DCM-0001", payload);
        ESP_LOGI(TAG, "Published stub status: %s", payload);
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void app_logic_init(void) {
    xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}
