/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: v8.8.1
 * Author:  R. Andrew Ballard (c) 2025
 *
 * Description:
 *   Application entry point:
 *   1) Initialize NVS
 *   2) Mount SPIFFS
 *   3) Start Wi-Fi manager (connect or provision)
 *   4) Hand off to app_logic_run() continuously
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "wifi_manager.h"   // wifi_credentials_exist(), wifi_manager_init(), wifi_manager_start()
#include "http_app.h"       // http_app_start()/stop()
#include "app_logic.h"      // app_logic_run()

static const char *TAG = "MAIN";

void app_main(void)
{
    // 1) Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2) Mount SPIFFS (for certificates, web assets, etc)
    {
        esp_vfs_spiffs_conf_t conf = {
            .base_path              = "/spiffs",
            .partition_label        = "spiffs",
            .max_files              = 5,
            .format_if_mount_failed = true
        };
        ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));
        ESP_LOGI(TAG, "SPIFFS mounted at '%s'", conf.base_path);
    }

    // 3) Initialize Wi-Fi manager internals
    ESP_LOGI(TAG, "Initializing Wi-Fi manager...");
    wifi_manager_init();

    // 4) Decide between STA connect or captive-portal
    if (wifi_credentials_exist()) {
        ESP_LOGI(TAG, "Credentials found → connecting STA");
        wifi_manager_start(); // this will enqueue CONNECT_STA
    } else {
        ESP_LOGW(TAG, "No credentials → starting provisioning");
        http_app_start(true); // SoftAP + captive portal
    }

    // 5) Application logic: repeatedly invoke run() to process your DCM loop
    ESP_LOGI(TAG, "Entering main application loop");
    while (true) {
        app_logic_run();
        // a small delay to avoid a tight spin if run() ever returns quickly
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
