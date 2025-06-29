/* 
 * File: main/main.c
 *
 * Entry point for the PianoGuard DCM-1 application.  
 * - Initializes NVS  
 * - Mounts SPIFFS for web UI assets  
 * - Handles Wi-Fi provisioning via SoftAP  
 * - Starts MQTT manager, HTTP server, and main logic  
 *
 * Created on:      10 June 2025 10:34:10  
 * Last edited on:  25 June 2025 12:00:00  
 *
 * Version: 8.7.0  
 * Author: R. Andrew Ballard (c) 2025  
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"

#include "board_manager.h"
#include "httpd_server.h"
#include "app_logic.h"

static const char *TAG = "PIANOGUARD_MAIN";

void app_main(void) {
    ESP_LOGI(TAG, "Starting PianoGuard Firmware v8.7.0");

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(board_manager_init());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_FLASH));
    wifi_config_t wifi_config;
    esp_wifi_get_config(ESP_IF_WIFI_STA, &wifi_config);

    if (strlen((const char*)wifi_config.sta.ssid) == 0) {
        ESP_LOGI(TAG, "Device not provisioned. Starting captive portal server...");
        start_http_server(); // This now handles creating the SoftAP and serving the UI
    } else {
        ESP_LOGI(TAG, "Device already provisioned. Connecting to %s", wifi_config.sta.ssid);
        esp_netif_create_default_wifi_sta();
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    }

    app_logic_init();
    ESP_LOGI(TAG, "app_main initialization complete.");
}


