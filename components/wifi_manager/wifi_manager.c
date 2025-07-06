/**
 * File:    components/esp32-wifi-manager/wifi_manager.c
 * Description: Wi-Fi manager implementation for PianoGuard DCM-1
 * Created on: 2025-06-18
 * Edited on:  2025-07-06
 * Version: v8.7.8
 * Author:  R. Andrew Ballard (c) 2025
 */

#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "http_app.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

static const char *TAG = "WIFI_MANAGER";

#define WIFI_MANAGER_QUEUE_SIZE 4

static QueueHandle_t wifi_manager_queue;
static esp_netif_t *netif_ap = NULL;
static esp_netif_t *netif_sta = NULL;

/* Event group to signal when Wi-Fi is connected */
static EventGroupHandle_t wifi_event_group;
const int CONNECTED_BIT = BIT0;

typedef enum {
    WIFI_MANAGER_MSG_CONNECT_STA = 0,
} wifi_manager_msg_code_t;

typedef struct {
    wifi_manager_msg_code_t msg_id;
} wifi_manager_message_t;

/**
 * Handle system Wi-Fi events
 */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                esp_wifi_connect();
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                esp_wifi_connect();
                ESP_LOGI(TAG, "Retrying Wi-Fi connection...");
                break;

            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "AP mode started");
                break;

            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP address.");
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

static void wifi_manager_task(void *param) {
    wifi_manager_message_t msg;
    while (1) {
        if (xQueueReceive(wifi_manager_queue, &msg, portMAX_DELAY)) {
            switch (msg.msg_id) {
                case WIFI_MANAGER_MSG_CONNECT_STA:
                    ESP_LOGI(TAG, "Connecting to STA...");
                    esp_wifi_set_mode(WIFI_MODE_STA);
                    esp_wifi_start();
                    break;
                default:
                    ESP_LOGW(TAG, "Unknown Wi-Fi message ID");
                    break;
            }
        }
    }
}

void wifi_manager_start() {
    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_CONNECT_STA
    };
    xQueueSend(wifi_manager_queue, &msg, portMAX_DELAY);
}

void wifi_manager_init() {
    ESP_LOGI(TAG, "Initializing Wi-Fi Manager…");

    wifi_event_group = xEventGroupCreate();
    wifi_manager_queue = xQueueCreate(WIFI_MANAGER_QUEUE_SIZE, sizeof(wifi_manager_message_t));

    netif_ap = esp_netif_create_default_wifi_ap();
    netif_sta = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "PianoGuard-Setup",
            .ssid_len = 0,
            .channel = 1,
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    xTaskCreate(wifi_manager_task, "wifi_manager_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Wi-Fi Manager initialized.");
}
