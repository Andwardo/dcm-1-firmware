/**
 * File:    main/main.c
 * Project: PianoGuard_DCM-1
 * Version: v8.7.8
 * Edited on: 2025-07-06
 * Author:  R. Andrew Ballard (c) 2025
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "wifi_manager.h"
#include "http_app.h"

static const char *TAG = "MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Initializing NVS flash...");
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    ESP_LOGI(TAG, "Setting up networking...");
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_manager_init();
    wifi_manager_start();

    http_app_start();
}
