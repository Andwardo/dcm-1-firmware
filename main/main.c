/*
 * File:    main.c
 * Project: PianoGuard_DCM-1
 * Version: v8.7.7
 * Author:  R. Andrew Ballard (c) 2025
 *
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
#include "wifi_manager.h"
#include "http_app.h"
#include "app_logic.h"    // <-- This must resolve now

static const char *TAG = "MAIN";

void app_main(void)
{
    // 1) Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    // 2) Mount SPIFFS
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path             = "/spiffs",
        .partition_label       = "spiffs",
        .max_files             = 5,
        .format_if_mount_failed= true
    };
    ESP_ERROR_CHECK( esp_vfs_spiffs_register(&spiffs_conf) );
    ESP_LOGI(TAG, "SPIFFS mounted at '%s'", spiffs_conf.base_path);

    // 3) Start Wi-Fi manager (will set up STA or SoftAP as needed)
    wifi_manager_start();
    ESP_LOGI(TAG, "Wi-Fi manager started");

    // 4) Kick off your application logic task
    app_logic_init();

    // main() returns to the RTOS – everything else now runs in tasks
}
