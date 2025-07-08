/*
 * main.c
 *
 * Created on: 2025-06-18
 * Edited on: 2025-07-08
 *     Author: R. Andrew Ballard
 *     Version: v8.2.47
 */

#include "wifi_manager.h"
#include "nvs_flash.h"
#include "esp_log.h"

#define TAG "MAIN"

void app_main(void) {
    ESP_LOGI(TAG, "Initializing NVS flash...");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "Starting Wi-Fi Manager…");

    // Check if credentials exist, otherwise start AP mode.
    // This is placeholder logic; replace with actual NVS read logic.
    bool creds_exist = false;

    if (creds_exist) {
        // Replace with real credential retrieval
        const char *ssid = "SavedSSID";
        const char *pass = "SavedPassword";
        ESP_LOGI(TAG, "Connecting to STA SSID: %s", ssid);
        // Replace with your actual function when implemented
        // wifi_manager_connect_sta(ssid, pass);
    } else {
        wifi_manager_start();  // Enters captive portal mode
    }
}