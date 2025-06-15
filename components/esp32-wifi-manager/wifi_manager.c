/*
 *  File: components/esp32-wifi-manager/wifi_manager.c
 *
 *  Created on: 2025-06-13 11:10:00
 *  Edited on:  2025-06-15 10:15:00 (CDT)
 *
 *  Author: R. Andrew Ballard (c) 2025
 *  Version: v8.3.0
 *
 *  Description:
 *    Wi-Fi manager for ESP32, production-ready. Attempts auto-connect to saved
 *    SSID in NVS. Falls back to captive portal if not found or if GPIO 27 is held low at boot.
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "wifi_manager.h"
#include "http_app.h"

#define RESET_GPIO              GPIO_NUM_27
#define RESET_HOLD_TIME_MS      3000
#define MAX_SSID_LEN            32
#define MAX_PASS_LEN            64

static const char *TAG = "WIFI_MANAGER";
static EventGroupHandle_t s_wifi_event_group = NULL;
static wifi_connected_cb_t s_on_connect_cb = NULL;

/* Wi-Fi event handler */
static void wifi_event_handler(void* arg, esp_event_base_t base, int32_t id, void* data)
{
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "STA start → connecting...");
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "Disconnected → reconnecting...");
        esp_wifi_connect();
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        if (s_on_connect_cb) s_on_connect_cb();
    }
}

/* Check if RESET_GPIO held low > RESET_HOLD_TIME_MS */
static bool detect_reset_hold(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = BIT64(RESET_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);

    int held_ms = 0;
    while (gpio_get_level(RESET_GPIO) == 0 && held_ms < RESET_HOLD_TIME_MS) {
        vTaskDelay(pdMS_TO_TICKS(10));
        held_ms += 10;
    }

    return held_ms >= RESET_HOLD_TIME_MS;
}

/* Erase saved credentials from NVS */
static void erase_wifi_credentials(void)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("wifi_creds", NVS_READWRITE, &nvs);
    if (err == ESP_OK) {
        nvs_erase_all(nvs);
        nvs_commit(nvs);
        nvs_close(nvs);
        ESP_LOGW(TAG, "Wi-Fi credentials erased from NVS");
    } else {
        ESP_LOGE(TAG, "Failed to open NVS for erase");
    }
}

/* Load Wi-Fi credentials from NVS */
static bool load_wifi_credentials(wifi_config_t *cfg)
{
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("wifi_creds", NVS_READONLY, &nvs);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "No saved credentials");
        return false;
    }

    size_t ssid_len = MAX_SSID_LEN;
    size_t pass_len = MAX_PASS_LEN;
    err = nvs_get_str(nvs, "ssid", (char *)cfg->sta.ssid, &ssid_len);
    if (err != ESP_OK) {
        nvs_close(nvs);
        return false;
    }

    err = nvs_get_str(nvs, "pass", (char *)cfg->sta.password, &pass_len);
    if (err != ESP_OK) {
        nvs_close(nvs);
        return false;
    }

    cfg->sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    cfg->sta.pmf_cfg.capable = true;
    cfg->sta.pmf_cfg.required = false;

    nvs_close(nvs);
    return true;
}

/* Initialize the Wi-Fi manager */
void wifi_manager_init(void)
{
    ESP_LOGI(TAG, "Wi-Fi Manager init");

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    s_wifi_event_group = xEventGroupCreate();
}

/* Start Wi-Fi, erase if long-press held, or fallback to AP mode */
void wifi_manager_start(wifi_connected_cb_t cb)
{
    s_on_connect_cb = cb;

    if (detect_reset_hold()) {
        ESP_LOGW(TAG, "RESET_SW held → clearing credentials & rebooting");
        erase_wifi_credentials();
        vTaskDelay(pdMS_TO_TICKS(500));
        esp_restart();
    }

    wifi_config_t config = { 0 };
    if (load_wifi_credentials(&config)) {
        ESP_LOGI(TAG, "Connecting to saved SSID: %s", config.sta.ssid);
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &config));
        ESP_ERROR_CHECK(esp_wifi_start());
    } else {
        ESP_LOGW(TAG, "No credentials → starting captive portal");
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
        wifi_config_t ap_cfg = {
            .ap = {
                .ssid = "PianoGuard-Setup",
                .ssid_len = 0,
                .password = "",
                .max_connection = 4,
                .authmode = WIFI_AUTH_OPEN
            },
        };
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_cfg));
        ESP_ERROR_CHECK(esp_wifi_start());
        http_app_start();
    }
}

/* Provide event group to wait on connection */
EventGroupHandle_t wifi_event_group_handle(void)
{
    return s_wifi_event_group;
}
