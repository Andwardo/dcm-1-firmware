/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: dc0736d-dirty
 * Author:  R. Andrew Ballard (c) 2025
 *  v 8.7.7
 * Description:
 *   Application entry point:
 *   - Init NVS
 *   - Mount SPIFFS
 *   - Start Wi-Fi manager (handles connect or provisioning)
 *   - Hand off to app logic
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"   // for wifi_manager_start()
#include "app_logic.h"      // for app_logic_init() / app_logic_run()

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
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path              = "/spiffs",  // must match partition label
        .partition_label        = "spiffs",
        .max_files              = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Kick off Wi-Fi manager (auto-chooses connect vs provisioning)
    wifi_manager_start();
    ESP_LOGI(TAG, "Wi-Fi manager started");

    // 4) Start application logic
    app_logic_init();

    // 5) If you prefer a run loop rather than a task:
    //    while (1) { app_logic_run(); }
    //    but app_logic_init has already spawned its own task.
}
