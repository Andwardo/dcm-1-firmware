/*
 * File: main/main.c
 *
 * Created on: 13 June 2025 09:00:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.1.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "app_logic.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "PianoGuard DCM-1 Starting Up");

    // Initialize Non-Volatile Storage (NVS)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "NVS Initialized");

    // Start the main application logic task.
    app_logic_start();

    ESP_LOGI(TAG, "app_main() finished. Handing over to FreeRTOS scheduler.");
}
