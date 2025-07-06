/*
 * wifi_manager.c
 *
 * Created on: 2025-06-18
 * Edited on: 2025-07-06
 *     Author: R. Andrew Ballard
 *     Version: v8.2.34a
 */

#include "wifi_manager.h"
#include "http_app.h"

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include <string.h>

#define TAG "WIFI_MANAGER"
#define WIFI_MANAGER_TASK_STACK_SIZE 4096
#define WIFI_MANAGER_TASK_PRIORITY   3
#define WIFI_MANAGER_QUEUE_SIZE      5

static QueueHandle_t wifi_manager_queue = NULL;
static TaskHandle_t wifi_manager_task_handle = NULL;
static wifi_config_t sta_config;

static EventGroupHandle_t wifi_event_group = NULL;
static esp_netif_t *netif_sta = NULL;

static void wifi_manager_task(void *param);

BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg) {
    if (!wifi_manager_queue || !msg) {
        return pdFAIL;
    }
    return xQueueSend(wifi_manager_queue, msg, portMAX_DELAY);
}

void wifi_manager_start(void) {
    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_START_PROVISIONING
    };
    wifi_manager_send_message(&msg);
}

void wifi_manager_connect_sta(const char *ssid, const char *password) {
    memset(&sta_config, 0, sizeof(wifi_config_t));
    strncpy((char *)sta_config.sta.ssid, ssid, sizeof(sta_config.sta.ssid) - 1);
    strncpy((char *)sta_config.sta.password, password, sizeof(sta_config.sta.password) - 1);

    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_CONNECT_STA
    };
    wifi_manager_send_message(&msg);
}

void wifi_manager_init(void) {
    // Initialize networking core and event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Create default interfaces for AP and STA
    esp_netif_create_default_wifi_ap();
    netif_sta = esp_netif_create_default_wifi_sta();

    // Create event group
    wifi_event_group = xEventGroupCreate();

    // Initialize Wi-Fi driver
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Initialize message queue and task
    wifi_manager_queue = xQueueCreate(WIFI_MANAGER_QUEUE_SIZE, sizeof(wifi_manager_message_t));
    xTaskCreate(wifi_manager_task, "wifi_manager_task", WIFI_MANAGER_TASK_STACK_SIZE, NULL, WIFI_MANAGER_TASK_PRIORITY, &wifi_manager_task_handle);
}

EventGroupHandle_t wifi_manager_get_event_group(void) {
    return wifi_event_group;
}

esp_netif_t *wifi_manager_get_esp_netif_sta(void) {
    return netif_sta;
}

bool wifi_credentials_exist(void) {
    // Optional: implement a check against NVS if credentials were stored
    return false;
}

static void wifi_manager_task(void *param) {
    wifi_manager_message_t msg;

    for (;;) {
        if (xQueueReceive(wifi_manager_queue, &msg, portMAX_DELAY)) {
            switch (msg.msg_id) {
                case WIFI_MANAGER_MSG_START_PROVISIONING:
                    ESP_LOGI(TAG, "Starting AP for provisioning...");
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
                    ESP_ERROR_CHECK(esp_wifi_start());
                    http_app_start(true);
                    break;

                case WIFI_MANAGER_MSG_CONNECT_STA:
                    ESP_LOGI(TAG, "Connecting to STA...");
                    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
                    ESP_ERROR_CHECK(esp_wifi_start());
                    ESP_ERROR_CHECK(esp_wifi_connect());
                    break;

                default:
                    ESP_LOGW(TAG, "Unknown message ID: %d", msg.msg_id);
                    break;
            }
        }
    }
}
