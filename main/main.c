/*
 * File: main/main.c
 * Description: Main application entry point and controller task.
 *
 * Created on: 2025-06-15
 * Edited on:  2025-06-19
 *
 * Version: v8.2.16
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"

// Project Components
#include "app_logic.h"
#include "wifi_manager.h"
#include "http_app.h"
#include "board_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_CONTROLLER";
#define NVS_NAMESPACE_WIFI "wifi_cred"

void app_main(void) {
    ESP_LOGI(TAG, ">> DCM-1 v8.2.16 booting...");

    // 1. Initialize all core services FIRST.
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 2. Initialize our components. This starts their respective tasks.
    board_manager_init();
    wifi_manager_init(); // This starts the wifi_manager_task
    app_logic_init();    // This starts the app_logic_task

    // 3. --- Check for saved credentials and decide application state ---
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_WIFI, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        // --- STATE: PROVISIONING MODE ---
        ESP_LOGI(TAG, "No credentials in NVS. Starting provisioning.");
        
        // Command the wifi_manager to start the Access Point
        wifi_manager_message_t msg = { .msg_id = WIFI_MANAGER_MSG_START_PROVISIONING };
        wifi_manager_send_message(&msg);
        
        // Wait for the wifi_manager to signal that the AP is ready
        ESP_LOGI(TAG, "Waiting for Wi-Fi AP to be ready...");
        EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
        xEventGroupWaitBits(wifi_events, WIFI_MANAGER_AP_STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        
        ESP_LOGI(TAG, "AP is ready. Starting provisioning web server.");
        http_app_start_provisioning_server();
    } else {
        // --- STATE: STATION (NORMAL) MODE ---
        ESP_LOGI(TAG, "Found credentials. Starting station mode.");
        
        wifi_manager_message_t msg = { .msg_id = WIFI_MANAGER_MSG_CONNECT_STA };
        size_t len_ssid = sizeof(msg.sta_config.sta.ssid);
        size_t len_pass = sizeof(msg.sta_config.sta.password);

        nvs_get_str(nvs_handle, "ssid", (char *)msg.sta_config.sta.ssid, &len_ssid);
        nvs_get_str(nvs_handle, "password", (char *)msg.sta_config.sta.password, &len_pass);
        nvs_close(nvs_handle);

        // Command the wifi_manager to connect
        wifi_manager_send_message(&msg);
    }

    ESP_LOGI(TAG, "Controller setup complete. Main task is now idle.");
    while(1) {
        vTaskDelay(portMAX_DELAY);
    }
}
