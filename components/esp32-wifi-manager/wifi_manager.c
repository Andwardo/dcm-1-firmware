/*
 * File: components/esp32-wifi-manager/wifi_manager.c
 *
 * Created on: 11 June 2025 21:15:00
 * Last edited on: 12 June 2025 10:20:00
 *
 * Version: 7.5.6
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "wifi_manager.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "esp_http_server.h"
#include "esp_mac.h"
#include "esp_system.h"
#include "http_app.h" // THIS IS THE REQUIRED INCLUDE

static const char TAG[] = "WIFI_MANAGER";
static wifi_connected_event_callback_t on_connected_callback = NULL;
static bool is_connected = false;

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
static void start_provisioning_ap(void);

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
	if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
		ESP_LOGI(TAG, "A station connected to the AP.");
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		ESP_LOGI(TAG, "A station disconnected from the AP.");
	} else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if(is_connected){
            is_connected = false;
            ESP_LOGE(TAG, "Disconnected from AP. Retrying...");
        }
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        if(!is_connected){
            is_connected = true;
            ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
            ESP_LOGI(TAG, "Got IP address:" IPSTR, IP2STR(&event->ip_info.ip));
            if(on_connected_callback) {
                on_connected_callback();
            }
        }
    }
}

static void start_provisioning_ap(void) {
    ESP_LOGI(TAG, "Starting Provisioning AP 'PianoGuard_DCM-1'");
    wifi_config_t ap_config = {
        .ap = {
            .ssid = "PianoGuard_DCM-1",
            .password = "",
            .authmode = WIFI_AUTH_OPEN,
            .max_connection = 4
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    http_app_start();
    ESP_LOGI(TAG, "Web server for Wi-Fi provisioning is running.");
}

void wifi_manager_start(wifi_connected_event_callback_t cb) {
    on_connected_callback = cb;

    ESP_LOGI(TAG, "Starting Wi-Fi Manager");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));

    wifi_config_t saved_config;
    if (esp_wifi_get_config(ESP_IF_WIFI_STA, &saved_config) == ESP_OK && strlen((const char*)saved_config.sta.ssid) > 0) {
        ESP_LOGI(TAG, "Found saved credentials for %s. Connecting...", saved_config.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &saved_config));
        ESP_ERROR_CHECK(esp_wifi_start());
    } else {
        ESP_LOGW(TAG, "No saved credentials found. Starting provisioning mode.");
        start_provisioning_ap();
    }
}

void wifi_manager_save_credentials_and_restart(const char* ssid, const char* password) {
    ESP_LOGI(TAG, "Received credentials for %s. Saving and restarting.", ssid);
    wifi_config_t wifi_config = { 0 };
    strcpy((char*)wifi_config.sta.ssid, ssid);
    strcpy((char*)wifi_config.sta.password, password);
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "Credentials saved. Restarting device...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
}
