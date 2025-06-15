// File: components/mqtt_manager/mqtt_manager.c
//
// Created on: 2025-06-15
// Edited  on: 2025-06-15 (CDT)
// Version:   v1.1
// Author:    R. Andrew Ballard (c) 2025
//

#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"    // ESP-IDF’s MQTT client API

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t s_client = NULL;

void mqtt_manager_init(void)
{
    // Configure your MQTT client here (URL, credentials, etc.)
    esp_mqtt_client_config_t cfg = {
        .broker = {
            .address = {
                .uri = CONFIG_MQTT_BROKER_URI
            }
        }
        // You can also set .credentials.username / .credentials.authentication if needed
    };
    s_client = esp_mqtt_client_init(&cfg);
    ESP_LOGI(TAG, "Initialized MQTT client");
}

void mqtt_manager_start(void)
{
    if (s_client == NULL) {
        mqtt_manager_init();
    }
    esp_mqtt_client_start(s_client);
    ESP_LOGI(TAG, "Started MQTT client");
}

void mqtt_manager_publish(const char *topic, const char *payload)
{
    if (s_client == NULL) {
        ESP_LOGW(TAG, "Client not started; initializing now");
        mqtt_manager_init();
        esp_mqtt_client_start(s_client);
    }
    int msg_id = esp_mqtt_client_publish(s_client, topic, payload, 0, 1, 0);
    if (msg_id >= 0) {
        ESP_LOGI(TAG, "Published msg_id=%d to %s", msg_id, topic);
    } else {
        ESP_LOGE(TAG, "Failed to publish to %s", topic);
    }
}
