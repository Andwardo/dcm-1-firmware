/**
 * File:    main/main.c
 * Project: PianoGuard_DCM-1
 * Version: e60e07a-dirty
 * Edited on: 2025-07-03
 * v8.7.7
 * Author:  R. Andrew Ballard (c) 2025
 *
 *   Application entry point:
 *     - Init NVS
 *     - Mount SPIFFS
 *     - Start Wi-Fi manager (handles both STA and captive-portal)
 *     - Hand off to app_logic_run()
 */

#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"
#include "app_logic.h"

static const char* TAG = "MAIN";

void app_main(void)
{
    // 1) NVS init
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS truncated; erasing and retrying...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2) SPIFFS
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path              = "/spiffs",
        .partition_label        = "spiffs",
        .max_files              = 25,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Start Wi-Fi manager (will choose STA vs provisioning)
    wifi_manager_start();
    ESP_LOGI(TAG, "Wi-Fi manager started");

    // 4) Run main application logic
    app_logic_run();
}
