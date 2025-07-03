/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: dc0736d-dirty → v8.7.8
 * Author:  R. Andrew Ballard (c) 2025
 * Description:
 *   Application entry point:
 *     1) Init NVS
 *     2) Mount SPIFFS
 *     3) Start Wi-Fi Manager (connect or captive-portal)
 *     4) Launch application logic
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"
#include "app_logic.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    // 1) Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS truncated, erasing and retrying...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2) Mount SPIFFS
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path              = "/spiffs",
        .partition_label        = "spiffs",
        .max_files              = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Start the Wi-Fi manager (handles netif, event-loop, connect or provisioning)
    ESP_LOGI(TAG, "Starting Wi-Fi manager");
    wifi_manager_start();

    // 4) Launch application logic task
    ESP_LOGI(TAG, "Starting application logic");
    app_logic_init();
}
