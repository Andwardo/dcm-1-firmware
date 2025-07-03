/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: v8.7.8
 * Author:  R. Andrew Ballard (c) 2025
 * Created:
 * Last edited:  3 Jul 2025
 * Description:
 *   Application entry point:
 *     1) Initialize NVS
 *     2) Mount SPIFFS
 *     3) Start Wi-Fi manager (with captive portal if needed)
 *     4) Start the main application logic task
 */

#include <stdio.h>
#include <string.h>

#include "esp_log.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "wifi_manager.h"   // for wifi_credentials_exist(), wifi_manager_init(), etc.
#include "http_app.h"       // for http_app_start()/stop()
#include "app_logic.h"      // for app_logic_init()/app_logic_run()

static const char *TAG = "MAIN";

void app_main(void)
{
    esp_err_t err;

    // 1. Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2. Mount SPIFFS (for certificates, web assets, etc.)
    {
        esp_vfs_spiffs_conf_t spiffs_conf = {
            .base_path              = "/spiffs",
            .partition_label        = "spiffs",
            .max_files              = 5,
            .format_if_mount_failed = true
        };
        ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));
        ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);
    }

    // 3. Initialize Wi-Fi manager
    wifi_manager_init();
    ESP_LOGI(TAG, "Wi-Fi Manager initialized");

    // 4. If no stored credentials, launch captive portal
    if (!wifi_credentials_exist()) {
        ESP_LOGW(TAG, "No Wi-Fi credentials found → starting captive portal");
        http_app_start(true);
    }

    // 5. Start application logic
    //    This spawns the app_logic FreeRTOS task.
    app_logic_init();
    ESP_LOGI(TAG, "Application logic task created");

    // 6. Hand off to app_logic_run() in a tight loop.
    //    This allows you to control execution flow from here if needed.
    while (1) {
        app_logic_run();
        // You can uncomment the delay if your logic run is too tight:
        // vTaskDelay(pdMS_TO_TICKS(100));
    }
}
