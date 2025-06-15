/*
 * File: components/app_logic/app_logic.c
 *
 * Created on: 2025-06-11 21:20:00
 * Edited on:  2025-06-15 14:45:00 CDT
 *
 * Version: v7.8.0
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#define LOG_LOCAL_LEVEL ESP_LOG_INFO

#include "app_logic.h"

/* Standard library includes */
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

/* FreeRTOS */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

/* ESP-IDF logging and drivers */
#include "esp_log.h"
#include "driver/gpio.h"

/* Project components */
#include "board_manager.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_LOGIC";

/**
 * @brief DCM sensor state representation
 */
typedef struct {
    bool power;
    bool water;
    bool pads;
} dcm_status_t;

/**
 * @brief Stub sensor reader
 *
 * Simulates sensor input while waiting for real GPIO integration.
 */
static dcm_status_t read_dcm_inputs_stub(void)
{
    static bool toggle = false;
    toggle = !toggle;

    dcm_status_t state = {
        .power = true,           /* Always on */
        .water = toggle,         /* Toggles */
        .pads  = !toggle         /* Inverse of water */
    };

    return state;
}

/**
 * @brief Main application task
 */
static void app_task(void *pvParameter)
{
    ESP_LOGI(TAG, "Application task started. Waiting for Wi-Fi connection...");

    /* Wait indefinitely for Wi-Fi connection */
    xEventGroupWaitBits(
        wifi_event_group_handle(),
        WIFI_CONNECTED_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY
    );

    ESP_LOGI(TAG, "Wi-Fi is connected. Starting MQTT Manager...");
    mqtt_manager_start();

    ESP_LOGI(TAG, "Main application loop running.");
    while (1) {
        /* --------------------------------------------------------------------- */
        /* 🧪 Simulated sensor state (stub logic) */
        dcm_status_t status = read_dcm_inputs_stub();

        /* --------------------------------------------------------------------- */
        /* 📤 JSON payload using simulated values */
        const char *device_id = "DCM-0001";
        time_t now = time(NULL);
        char payload[256];
        snprintf(
            payload, sizeof(payload),
            "{\"device_id\":\"%s\",\"timestamp\":%lld,\"status\":{"
            "\"power\":%s,\"water\":%s,\"pads\":%s}}",
            device_id, (long long)now,
            status.power ? "true" : "false",
            status.water ? "true" : "false",
            status.pads  ? "true" : "false"
        );

        mqtt_manager_publish("dcm/status/DCM-0001", payload);
        ESP_LOGI(TAG, "Published dummy status: %s", payload);

        /* --------------------------------------------------------------------- */
        /* 💤 Delay between cycles */
        vTaskDelay(pdMS_TO_TICKS(10000));  /* 10s polling interval */
    }
}

/**
 * @brief Start the application logic task
 */
void app_logic_start_task(void)
{
    xTaskCreate(app_task, "app_task", configMINIMAL_STACK_SIZE * 5, NULL, 5, NULL);
}
