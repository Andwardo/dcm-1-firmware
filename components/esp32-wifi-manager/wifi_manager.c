/*
 * File: components/esp32-wifi-manager/wifi_manager.c
 * Description: Message-driven Wi-Fi management component.
 *
 * Created on: 2025-06-18
 * Edited on:  2025-06-29
 *
 * Version: v8.2.18
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "http_app.h"
#include "wifi_manager.h"

static const char *TAG = "WIFI_MANAGER";
#define WIFI_PROV_SSID "PianoGuard-Setup"

static QueueHandle_t s_wifi_manager_queue;
static EventGroupHandle_t s_wifi_event_group;

static void wifi_manager_task(void *pvParameters);
static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

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

static void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
    if (event_base == WIFI_EVENT) {
        if (event_id == WIFI_EVENT_AP_START) {
            ESP_LOGI(TAG, "Event: SoftAP Started");
            xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_AP_STARTED_BIT);
            } 
	    else if (event_id == WIFI_EVENT_STA_DISCONNECTED) {
            ESP_LOGW(TAG, "Event: Station disconnected. Retrying...");
            xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_STA_DISCONNECTED_BIT);
            xEventGroupClearBits(s_wifi_event_group, WIFI_MANAGER_STA_CONNECTED_BIT);
            esp_wifi_connect();
            }
	} 
	else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
            ESP_LOGI(TAG, "Event: Got IP, Wi-Fi is connected");
            xEventGroupSetBits(s_wifi_event_group, WIFI_MANAGER_STA_CONNECTED_BIT);
            xEventGroupClearBits(s_wifi_event_group, WIFI_MANAGER_STA_DISCONNECTED_BIT);
        }
    }

static void wifi_manager_task(void *pvParameters) {
    // THE FIX: The core network stack is initialized in main.c now.
    // This task ONLY initializes the Wi-Fi driver and interfaces.
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));

    wifi_manager_message_t msg;
    ESP_LOGI(TAG, "Task started, waiting for messages.");

        for (;;) {
            if (xQueueReceive(s_wifi_manager_queue, &msg, portMAX_DELAY)) {
                switch (msg.msg_id) {
                    case WIFI_MANAGER_MSG_START_PROVISIONING:
                        ESP_LOGI(TAG, "Handling MSG: START_PROVISIONING");
                        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
                        wifi_config_t ap_config = {0};
                        ap_config.ap.max_connection = 4;
                        ap_config.ap.authmode = WIFI_AUTH_OPEN;
                        strlcpy((char*)ap_config.ap.ssid, WIFI_PROV_SSID, sizeof(ap_config.ap.ssid));
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
                        ESP_ERROR_CHECK(esp_wifi_start());
                        break;
                    case WIFI_MANAGER_MSG_CONNECT_STA:
                        ESP_LOGI(TAG, "Handling MSG: CONNECT_STA to '%s'", msg.sta_config.sta.ssid);
                        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &msg.sta_config));
                        ESP_ERROR_CHECK(esp_wifi_start());
                        esp_wifi_connect();
                        break;
                }
            } 
        }
