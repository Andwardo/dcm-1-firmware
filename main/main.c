/*
 * File: main/main.c
 * Version: v8.2.9
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "app_logic.h"
#include "wifi_manager.h"
#include "http_app.h"
#include "board_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "APP_CONTROLLER";
#define NVS_NAMESPACE_WIFI "wifi_cred"

void app_main(void) {
    ESP_LOGI(TAG, ">> DCM-1 v8.2.9 booting...");
    ESP_ERROR_CHECK(nvs_flash_init());

    board_manager_init();
    wifi_manager_init();
    app_logic_init();

    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE_WIFI, NVS_READONLY, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "No credentials in NVS. Starting provisioning.");
        wifi_manager_send_message(WIFI_MANAGER_MSG_START_PROVISIONING, NULL);
        EventGroupHandle_t wifi_events = wifi_manager_get_event_group();
        xEventGroupWaitBits(wifi_events, WIFI_MANAGER_AP_STARTED_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
        ESP_LOGI(TAG, "AP is ready. Starting web server.");
        http_app_start_provisioning_server();
    } else {
        ESP_LOGI(TAG, "Found credentials. Starting station mode.");
        wifi_config_t wifi_config = {0};
        size_t len_ssid = sizeof(wifi_config.sta.ssid);
        size_t len_pass = sizeof(wifi_config.sta.password);
        nvs_get_str(nvs_handle, "ssid", (char *)wifi_config.sta.ssid, &len_ssid);
        nvs_get_str(nvs_handle, "password", (char *)wifi_config.sta.password, &len_pass);
        nvs_close(nvs_handle);
        wifi_manager_send_message(WIFI_MANAGER_MSG_CONNECT_STA, &wifi_config);
    }
    
    ESP_LOGI(TAG, "Controller setup complete. Main task is now idle.");
    // An app_main task should not return. It idles here while other tasks run.
    while(1) {
        vTaskDelay(portMAX_DELAY);
    }
}
