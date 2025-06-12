/*
 * File: main/main.c
 *
 * Created on: 11 June 2025 08:40:00
 * Last edited on: 12 June 2025 21:40:00
 *
 * Version: 7.6.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "app_logic.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "PianoGuard DCM-1 Starting Up...");
    
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    app_logic_start_task();

    ESP_LOGI(TAG, "Bootstrapping complete.");
}
