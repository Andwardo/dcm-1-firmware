/*
 *  main.c
 *
 *  Created on: 2025-06-18
 *  Edited on: 2025-07-09
 *      Author: R. Andrew Ballard
 *      Version: v8.2.33
 */

#include "wifi_manager.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG "MAIN"

void app_main(void) {
    ESP_LOGI(TAG, "Initializing NVS flash...");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "Starting Wi-Fi Manager…");

    bool creds_exist = false;

    if (creds_exist) {
        wifi_manager_connect_sta("SavedSSID", "SavedPassword");
    } else {
        wifi_manager_start();  // Enters captive portal mode
    }
}