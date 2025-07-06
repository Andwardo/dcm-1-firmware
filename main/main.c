/**
 * File: main.c
 * Description: PianoGuard DCM entry point
 * Created on: 2025-06-25
 * Edited on:  2025-07-07 → v8.7.10 (NVS error handling, startup order fix)
 * Author: R. Andrew Ballard (c) 2025
 */

#include "wifi_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";

void app_main(void) {
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing NVS flash...");
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS error 0x%x, erasing...", err);
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    } else {
        ESP_ERROR_CHECK(err);
    }

    ESP_LOGI(TAG, "Initializing Wi-Fi Manager…");
    wifi_manager_init();

    ESP_LOGI(TAG, "Starting Wi-Fi Manager…");
    wifi_manager_start();
}
