/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: 8.7.7
 * Author:  R. Andrew Ballard (c) 2025
 *
 * Description:
 *   Application entry point:
 *     - Initialize NVS
 *     - Mount SPIFFS
 *     - Start Wi-Fi manager (which connects or launches provisioning)
 *     - Initialize main application logic task
 */

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"   // for wifi_manager_start()
#include "http_app.h"       // for http_app_start()/stop(), used internally
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

    // 2) Mount SPIFFS
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path              = "/spiffs",
        .partition_label        = "spiffs",
        .max_files              = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Start Wi-Fi manager (handles credentials vs. provisioning)
    ESP_LOGI(TAG, "Starting Wi-Fi manager");
    wifi_manager_start();

    // 4) Kick off the main application logic task
    ESP_LOGI(TAG, "Starting application logic");
    app_logic_init();

    // 5) Let the OS take over—just idle here
    for (;; ) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
