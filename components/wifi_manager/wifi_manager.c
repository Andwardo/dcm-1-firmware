/**
 * File: wifi_manager.c
 * Description: Wi-Fi manager with SoftAP provisioning and persistent STA config
 * Created on: 2025-06-25
 * Edited on:  2025-07-07 → v8.7.10 (proper init/start separation, no port reuse bug)
 * Author: R. Andrew Ballard (c) 2025
 */

#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "dns_server.h"
#include "http_app.h"

static const char *TAG = "WIFI_MANAGER";
static QueueHandle_t wifi_event_queue = NULL;
static bool wifi_initialized = false;

typedef struct {
    wifi_config_t sta;
} wifi_sta_config_t;

typedef struct {
    int msg_id;
    wifi_sta_config_t sta_config;
} wifi_manager_message_t;

#define WIFI_MANAGER_QUEUE_SIZE 4
#define WIFI_MANAGER_MSG_CONNECT_STA 1

static void wifi_task(void *pvParameters) {
    wifi_manager_message_t msg;

    for (;;) {
        if (xQueueReceive(wifi_event_queue, &msg, portMAX_DELAY)) {
            if (msg.msg_id == WIFI_MANAGER_MSG_CONNECT_STA) {
                ESP_LOGI(TAG, "Connecting to SSID: %s", (char *)msg.sta_config.sta.sta.ssid);
                ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &msg.sta_config.sta));
                ESP_ERROR_CHECK(esp_wifi_connect());
            }
        }
    }
}

esp_err_t wifi_manager_send_message(void *msg_ptr) {
    if (!wifi_event_queue) return ESP_ERR_INVALID_STATE;
    return xQueueSend(wifi_event_queue, msg_ptr, portMAX_DELAY);
}

void wifi_manager_init(void) {
    if (wifi_initialized) {
        ESP_LOGW(TAG, "Wi-Fi Manager already initialized");
        return;
    }

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_event_queue = xQueueCreate(WIFI_MANAGER_QUEUE_SIZE, sizeof(wifi_manager_message_t));
    xTaskCreatePinnedToCore(wifi_task, "wifi_task", 4096, NULL, 3, NULL, tskNO_AFFINITY);

    wifi_initialized = true;
}

void wifi_manager_start(void) {
    if (!wifi_initialized) {
        ESP_LOGE(TAG, "Cannot start Wi-Fi Manager before init");
        return;
    }

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "PianoGuard-Setup",
            .ssid_len = 0,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGW(TAG, "Starting SoftAP provisioning");

    ESP_LOGI(TAG, "Launching HTTP captive-portal");
    http_app_start(false);

    ESP_LOGI(TAG, "Launching DNS captive-portal");
    dns_server_start();
}
