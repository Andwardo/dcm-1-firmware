/*
 * PianoGuard_DCM-1 Main Application
 * Version: 9a8c37e-dirty
 *
 * (c) 2025 R. Andrew Ballard
 * All Rights Reserved.
 */

#include <stdio.h>
#include "esp_log.h"
#include "esp_spiffs.h"
#include "wifi_manager.h"   // for wifi_credentials_exist()
#include "app_logic.h"      // for app_logic_run()

// --- SPIFFS configuration fall-backs ---
#ifndef CONFIG_SPIFFS_BASE_PATH
#define CONFIG_SPIFFS_BASE_PATH  "/spiffs"
#endif

#ifndef CONFIG_SPIFFS_PARTITION_LABEL
#define CONFIG_SPIFFS_PARTITION_LABEL  "spiffs"
#endif

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "MAIN: Initializing NVS...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_LOGI(TAG, "MAIN: Mounting SPIFFS for certificate access...");
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path      = CONFIG_SPIFFS_BASE_PATH,
        .partition_label= CONFIG_SPIFFS_PARTITION_LABEL,
        .max_files      = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_vfs_spiffs_register(&spiffs_conf));
    ESP_LOGI(TAG, "MAIN: SPIFFS mounted at %s", CONFIG_SPIFFS_BASE_PATH);

    // If Wi-Fi creds already exist, skip provisioning
    if (!wifi_credentials_exist()) {
        ESP_LOGI(TAG, "MAIN: No Wi-Fi credentials—starting provisioning...");
        wifi_manager_start();
    } else {
        ESP_LOGI(TAG, "MAIN: Wi-Fi credentials found—connecting...");
        wifi_manager_start();  // May just call start with stored creds
    }

    // Once connectivity is up, hand off to your application logic
    ESP_LOGI(TAG, "MAIN: Running application logic...");
    app_logic_run();
}
