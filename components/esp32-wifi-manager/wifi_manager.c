/**
 * File: wifi_manager.c
 * Description: Message-driven Wi-Fi management component.
 *
 * Created on: 2025-06-18
 * Edited on:  2025-07-01
 * Version: v8.2.23
 * Author: R. Andrew Ballard (c) 2025
 */

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "http_app.h"
#include "wifi_manager.h"

#define WIFI_PROV_SSID "PianoGuard-Setup"

static const char *TAG = "WIFI_MANAGER";

// Static handles
static QueueHandle_t s_wifi_manager_queue = NULL;
static EventGroupHandle_t s_wifi_event_group = NULL;
static esp_netif_t *s_netif_sta = NULL;

// Forward declarations
static void wifi_manager_task(void *pvParameters);
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

// --- Internal functions ---

static void wifi_manager_start_softap(void) {
    wifi_config_t ap_config = {0};
    strlcpy((char *)ap_config.ap.ssid, WIFI_PROV_SSID, sizeof(ap_config.ap.ssid));
    ap_config.ap.max_connection = 4;
    ap_config.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void wifi_manager_connect_sta(const wifi_config_t *config) {
    ESP_LOGI(TAG, "Connecting to SSID: %s", config->sta.ssid);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, (wifi_config_t *)config));
    ESP_ERROR_CHECK(esp_wifi_start());

    esp_err_t err = esp_wifi_connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_connect() failed: %s", esp_err_to_name(err));
    }
}

void wifi_manager_start_provisioning(void) {
    wifi_manager_start_softap();
    http_app_start(false);
}

// --- Public API ---

void wifi_manager_init(void) {
    s_wifi_event_group = xEventGroupCreate();
    s_wifi_manager_queue = xQueueCreate(5, sizeof(wifi_manager_message_t));
    xTaskCreate(wifi_manager_task, "wifi_manager_task", 4096, NULL, 5, NULL);
}

BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg) {
    if (!msg) return pdFALSE;
    return xQueueSend(s_wifi_manager_queue, msg, portMAX_DELAY);
}

EventGroupHandle_t wifi_manager_get_event_group(void) {
    return s_wifi_event_group;
}

esp_netif_t *wifi_manager_get_esp_netif_sta(void) {
    return s_netif_sta;
}

void wifi_manager_connect_async(const char *ssid, const char *password) {
    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_CONNECT_STA
    };
    memset(&msg.sta_config, 0, sizeof(wifi_config_t));
    strlcpy((char *)msg.sta_config.sta.ssid, ssid, sizeof(msg.sta_config.sta.ssid));
    strlcpy((char *)msg.sta_config.sta.password, password, sizeof(msg.sta_config.sta.password));

    if (wifi_manager_send_message(&msg) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to send connect message to Wi-Fi manager");
    }
}

// --- Event Handler ---

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT) {
        switch (event_id) {
            case WIFI_EVENT_AP_START:
                ESP_LOGI(TAG, "Event: SoftAP Started");
                xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_AP_STARTED_BIT);
                break;

            case WIFI_EVENT_STA_DISCONNECTED:
                ESP_LOGW(TAG, "Event: Station disconnected. Retrying...");
                xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_STA_DISCONNECTED_BIT);
                xEventGroupClearBits(s_wifi_event_group, WIFI_MANAGER_STA_CONNECTED_BIT);
                esp_wifi_connect();
                break;
        }
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "Event: Got IP, Wi-Fi is connected");
        xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_STA_CONNECTED_BIT);
        xEventGroupClearBits(s_wifi_event_group, WIFI_MANAGER_STA_DISCONNECTED_BIT);
    }
}

// --- Task Loop ---

static void wifi_manager_task(void *pvParameters) {
    esp_netif_create_default_wifi_ap();  // Ignored return
    s_netif_sta = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    ESP_LOGI(TAG, "Wi-Fi manager task started.");

    wifi_manager_message_t msg;
    for (;;) {
        if (xQueueReceive(s_wifi_manager_queue, &msg, portMAX_DELAY)) {
            switch (msg.msg_id) {
                case WIFI_MANAGER_MSG_START_PROVISIONING:
                    ESP_LOGI(TAG, "Handling MSG: START_PROVISIONING");
                    wifi_manager_start_provisioning();
                    break;

                case WIFI_MANAGER_MSG_CONNECT_STA:
                    ESP_LOGI(TAG, "Handling MSG: CONNECT_STA to '%s'", msg.sta_config.sta.ssid);
                    wifi_manager_connect_sta(&msg.sta_config);
                    break;
            }
        }
    }
}
