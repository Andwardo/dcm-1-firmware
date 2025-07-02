
/**
 * File: main.c
 * Description: Main entry point for the PianoGuard DCM-1 application.
 * Version: v8.6.5 → updated 2025-07-07
 * Author: R. Andrew Ballard (c) 2025
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_spiffs.h"

#include "board_manager.h"
#include "wifi_manager.h"
#include "http_app.h"      // for http_app_start()
#include "dns_server.h"    // for dns_server_start()
#include "mqtt_manager.h"
#include "app_logic.h"

static const char *TAG = "MAIN";

static bool wifi_credentials_exist(void) {
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("nvs.net80211", NVS_READONLY, &nvs);
    if (err != ESP_OK) return false;

    uint8_t ssid[33] = {0};
    size_t required_size = sizeof(ssid);
    err = nvs_get_blob(nvs, "sta.ssid", ssid, &required_size);
    nvs_close(nvs);

    return (err == ESP_OK && strlen((char *)ssid) > 0);
}

void app_main(void) {
    ESP_LOGI(TAG, "Initializing NVS...");
    ESP_ERROR_CHECK(nvs_flash_init());

    ESP_LOGI(TAG, "Mounting SPIFFS for certificate access...");
    esp_vfs_spiffs_conf_t spiffs_conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 4,
        .format_if_mount_failed = false
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&spiffs_conf));

    ESP_LOGI(TAG, "Creating default event loop...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "Initializing board manager...");
    board_manager_init();

    ESP_LOGI(TAG, "Starting Wi-Fi manager (init + provisioning)...");
    wifi_manager_start();   // ← new API that does init()+start provisioning

    ESP_LOGI(TAG, "Waiting for Wi-Fi STA connection...");
    xEventGroupWaitBits(
        wifi_manager_get_event_group(),
        WIFI_MANAGER_STA_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY
    );

    ESP_LOGI(TAG, "Wi-Fi connected. Initializing rest of system...");
    app_logic_init();
    mqtt_manager_init();
}
