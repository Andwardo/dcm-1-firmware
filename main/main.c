/**
 * File: main.c
 * Description: Main entry point for the PianoGuard DCM-1 application.
 * Created on: 2025-06-25
 * Edited on:  2025-07-03
 * Version: v8.6.6
 * Author: R. Andrew Ballard (c) 2025
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "board_manager.h"
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

    ESP_LOGI(TAG, "Creating default event loop...");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ESP_LOGI(TAG, "Initializing board and Wi-Fi manager...");
    board_manager_init();
    wifi_manager_init();

    wifi_manager_message_t msg = {0};

    if (wifi_credentials_exist()) {
        ESP_LOGI(TAG, "Stored Wi-Fi credentials found. Attempting STA connection...");

        wifi_config_t config = {0};
        nvs_handle_t nvs;
        if (nvs_open("nvs.net80211", NVS_READONLY, &nvs) == ESP_OK) {
            size_t ssid_len = sizeof(config.sta.ssid);
            size_t pass_len = sizeof(config.sta.password);
            nvs_get_blob(nvs, "sta.ssid", config.sta.ssid, &ssid_len);
            nvs_get_blob(nvs, "sta.passwd", config.sta.password, &pass_len);
            nvs_close(nvs);
        }

        msg.msg_id = WIFI_MANAGER_MSG_CONNECT_STA;
        msg.sta_config = config;
    } else {
        ESP_LOGW(TAG, "No stored credentials. Starting provisioning...");
        msg.msg_id = WIFI_MANAGER_MSG_START_PROVISIONING;
    }

    wifi_manager_send_message(&msg);

    ESP_LOGI(TAG, "Waiting for Wi-Fi STA connection...");
    xEventGroupWaitBits(
        wifi_manager_get_event_group(),
        WIFI_MANAGER_STA_CONNECTED_BIT,
        pdFALSE,
        pdTRUE,
        portMAX_DELAY
    );

    ESP_LOGI(TAG, "Wi-Fi connected. Initializing application...");
    app_logic_init();
    mqtt_manager_init();
}
