/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: 8.7.7
 * Author:  R. Andrew Ballard (c) 2025
 * Description:
 *   Application entry point:
 *   - Init NVS
 *   - Mount SPIFFS
 *   - Start Wi-Fi manager (which will provision or connect)
 *   - Start main application logic task
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"   // for wifi_manager_start()
#include "app_logic.h"      // for app_logic_init()

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
        .base_path              = "/spiffs",
        .partition_label        = "spiffs",
        .max_files              = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK( esp_vfs_spiffs_register(&spiffs_conf) );
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Start the Wi-Fi manager (it will provision or auto-connect)
    wifi_manager_start();
    ESP_LOGI(TAG, "Wi-Fi manager running");

    // 4) Launch the application logic task
    app_logic_init();
    ESP_LOGI(TAG, "Application logic started");

    // 5) Suspend this task; all work is done in background tasks
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
