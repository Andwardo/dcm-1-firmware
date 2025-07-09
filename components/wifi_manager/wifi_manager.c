/*
 *  wifi_manager.c
 *
 *  Created on: 2025-06-23
 *  Edited on: 2025-07-09
 *      Author: Andwardo
 *      Version: v8.2.48
 */

#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string.h>

static const char *TAG = "wifi_manager";

static EventGroupHandle_t wifi_event_group;
static char ap_ssid[32] = {0};

EventGroupHandle_t wifi_manager_get_event_group(void) {
    return wifi_event_group;
}

void generate_ap_ssid_from_mac(void) {
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(ap_ssid, sizeof(ap_ssid), "PianoGuard-%02X%02X%02X", mac[3], mac[4], mac[5]);
    ESP_LOGI(TAG, "Generated AP SSID: %s", ap_ssid);
}

const char* get_ap_ssid(void) {
    return ap_ssid;
}

void wifi_manager_start(void) {
    ESP_LOGI(TAG, "Starting WiFi Manager");

    wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    generate_ap_ssid_from_mac();

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = "",
            .ssid_len = 0,
            .channel = 1,
            .password = "pianoguard",
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };

    strncpy((char *)wifi_config.ap.ssid, ap_ssid, sizeof(wifi_config.ap.ssid));
    wifi_config.ap.ssid_len = strlen(ap_ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi Manager started with SSID: %s", ap_ssid);
}