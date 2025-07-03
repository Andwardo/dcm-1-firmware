/**
 * File: wifi_manager.c
 * Description: Wi-Fi manager implementation for PianoGuard DCM-1
 * Created on: 2025-06-18
 * Edited on:  2025-07-03
 * Version: v8.6.11
 * Author: R. Andrew Ballard (c) 2025
 */

#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include <string.h>

// Captive-portal HTTP and DNS
#include "http_app.h"
#include "dns_server.h"

static const char *TAG = "WIFI_MANAGER";

#define WIFI_MANAGER_QUEUE_SIZE      4
#define WIFI_MANAGER_TASK_STACK_SIZE 4096
#define WIFI_MANAGER_TASK_PRIORITY   5

static QueueHandle_t wifi_manager_queue;
static EventGroupHandle_t wifi_event_group;
static esp_netif_t *netif_sta = NULL;
static esp_netif_t *netif_ap = NULL;

// Event handler for Wi-Fi and IP events
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_STA_START:
                ESP_LOGI(TAG, "STA start event");
                esp_wifi_connect();
                break;
            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "STA disconnected, retrying...");
                esp_wifi_connect();
                break;
            default:
                break;
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Got IP address");
        xEventGroupSetBits(wifi_event_group, WIFI_MANAGER_STA_CONNECTED_BIT);
    }
}

// Task for handling Wi-Fi manager messages
static void wifi_manager_task(void *param) {
    wifi_manager_message_t msg;
    while (1) {
        if (xQueueReceive(wifi_manager_queue, &msg, portMAX_DELAY)) {
            switch (msg.msg_id) {
                case WIFI_MANAGER_MSG_CONNECT_STA:
                    ESP_LOGI(TAG, "Connecting to SSID: %s", msg.sta_config.sta.ssid);
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &msg.sta_config));
                    ESP_ERROR_CHECK(esp_wifi_start());
                    break;
                case WIFI_MANAGER_MSG_START_PROVISIONING: {
                    ESP_LOGW(TAG, "Starting SoftAP provisioning");
                    wifi_config_t ap_config = {
                        .ap = {
                            .ssid = "PianoGuard-Setup",
                            .ssid_len = 0,
                            .channel = 1,
                            .password = "pianoguard",
                            .max_connection = 4,
                            .authmode = WIFI_AUTH_WPA_WPA2_PSK
                        }
                    };

                    if (strlen((char *)ap_config.ap.password) == 0) {
                        ap_config.ap.authmode = WIFI_AUTH_OPEN;
                    }

                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
                    ESP_ERROR_CHECK(esp_wifi_start());

                    // Start captive-portal services
                    ESP_LOGI(TAG, "Launching HTTP captive-portal");
                    http_app_start(true);
                    ESP_LOGI(TAG, "Launching DNS captive-portal");
                    dns_server_start();
                    break;
                }
                default:
                    ESP_LOGW(TAG, "Unknown message received in Wi-Fi Manager");
                    break;
            }
        }
    }
}

EventGroupHandle_t wifi_manager_get_event_group(void) {
    return wifi_event_group;
}

esp_netif_t *wifi_manager_get_esp_netif_sta(void) {
    return netif_sta;
}

BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg) {
    return xQueueSend(wifi_manager_queue, msg, pdMS_TO_TICKS(100));
}

void wifi_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi Manager...");

    wifi_manager_queue = xQueueCreate(WIFI_MANAGER_QUEUE_SIZE, sizeof(wifi_manager_message_t));
    wifi_event_group  = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    // Default event loop already created in main; do not recreate here

    netif_sta = esp_netif_create_default_wifi_sta();
    netif_ap  = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    xTaskCreate(wifi_manager_task, "wifi_manager_task", WIFI_MANAGER_TASK_STACK_SIZE, NULL,
                WIFI_MANAGER_TASK_PRIORITY, NULL);
}
/**
 * @brief Initialize the Wi-Fi manager internals, then trigger
 *        provisioning mode (SoftAP + captive-portal).
 */
void wifi_manager_start(void) {
    // 1. Run the top-level init (queue, netif, event handlers)
    wifi_manager_init();

    // 2. Enqueue the “start provisioning” message
    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_START_PROVISIONING
    };
    if (wifi_manager_send_message(&msg) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue provisioning message");
    }
}

/**
 * @brief   Return true if we already have stored STA credentials (a non-empty SSID).
 */
bool wifi_credentials_exist(void)
{
    wifi_config_t conf;
    // Ask the driver for the current STA config
    if (esp_wifi_get_config(WIFI_IF_STA, &conf) == ESP_OK) {
        // If SSID string length > 0, we have credentials
        return (strlen((char*)conf.sta.ssid) > 0);
    }
    // On error or empty SSID, treat as “no credentials”
    return false;
}
