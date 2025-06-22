/*
 * Project:    PianoGuard_DCM-1
 * File:       main/main.c
 * Version:    8.2.43D
 * Author:     R. Andrew Ballard
 * Date:       Jun 26 2025
 *
 * Bootloader + HTTP captive-portal with SPIFFS mounting.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"

#include "httpd_server.h"
#include "device_config.h"
#include "sensors.h"

static const char *TAG = "PianoGuard";

static void init_softap(void)
{
    ESP_LOGI(TAG, "Initializing SoftAP…");
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    esp_netif_init();
    esp_netif_create_default_wifi_ap();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    wifi_config_t ap_cfg = {
        .ap = {
            .ssid           = "PianoGuard_AP",
            .ssid_len       = 0,
            .channel        = 1,
            .authmode       = WIFI_AUTH_OPEN,
            .max_connection = 4,
            .password       = "",
        },
    };
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_AP, &ap_cfg) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_LOGI(TAG, "SoftAP \"%s\" started", ap_cfg.ap.ssid);
}

static void mount_spiffs(void)
{
    ESP_LOGI(TAG, "Mounting SPIFFS…");
    esp_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };
    ESP_ERROR_CHECK( esp_spiffs_register(&conf) );
    size_t total = 0, used = 0;
    ESP_ERROR_CHECK( esp_spiffs_info(NULL, &total, &used) );
    ESP_LOGI(TAG, "SPIFFS mounted: total=%u, used=%u", (unsigned)total, (unsigned)used);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Booting…");
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK( esp_event_loop_create_default() );
    init_softap();
    mount_spiffs();
    start_http_server();
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
