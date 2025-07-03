/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: dc0736d-dirty
 * Author:  R. Andrew Ballard (c) 2025
 *  v 8.7.6
 * Description:
 *   Application entry point: 
 *   - Init NVS
 *   - Mount SPIFFS
 *   - Start Wi-Fi manager + HTTP captive portal if needed
 *   - Hand off to app logic
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"   // for wifi_credentials_exist()
#include "http_app.h"       // for http_app_start()/stop()
#include "app_logic.h"      // for app_logic_init()/run()

static const char *TAG = "MAIN";

void app_main(void)
{
    // 1) Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing and retrying...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2) Mount SPIFFS (for certificates, web assets, etc)
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path      = "/spiffs",     // hard-coded; change if needed
        .partition_label= "spiffs",      // must match your partition table
        .max_files      = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK( esp_vfs_spiffs_register(&spiffs_conf) );
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Initialize Wi-Fi manager
    wifi_manager_init();
    ESP_LOGI(TAG, "Wi-Fi manager started");

    // 4) If no credentials, spawn captive-portal
    if (!wifi_credentials_exist()) {
        ESP_LOGW(TAG, "No Wi-Fi credentials stored, starting captive portal");
        http_app_start(true);
    }

    // 5) Initialize and run your main application logic
//    app_logic_init();
    while (1) {
        app_logic_run();
        // optionally vTaskDelay(pdMS_TO_TICKS(100));
    }
}
