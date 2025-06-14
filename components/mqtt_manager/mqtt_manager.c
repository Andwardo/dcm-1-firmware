/*
 * File: components/mqtt_manager/mqtt_manager.c
 *
 * Created on: 14 June 2025 09:30:00
 * Last edited on: 14 June 2025 10:25:00
 *
 * Version: 2.1
 */

#include "mqtt_manager.h"
#include "mqtt_client.h"
#include "esp_log.h"

static const char *TAG = "mqtt_mgr";
static esp_mqtt_client_handle_t client = NULL;

esp_err_t mqtt_manager_start(const char *uri)
{
    esp_mqtt_client_config_t cfg = {
        .uri = uri,
    };
    client = esp_mqtt_client_init(&cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
    ESP_LOGI(TAG, "MQTT client started on %s", uri);
    return ESP_OK;
}

esp_err_t mqtt_manager_publish(const char *topic, const char *payload)
{
    int msg_id = esp_mqtt_client_publish(client, topic, payload, 0, 1, 0);
    ESP_LOGI(TAG, "Published msg %d to %s", msg_id, topic);
    return (msg_id >= 0) ? ESP_OK : ESP_FAIL;
}

esp_err_t mqtt_manager_stop(void)
{
    ESP_LOGI(TAG, "Stopping MQTT client");
    ESP_ERROR_CHECK(esp_mqtt_client_stop(client));
    ESP_ERROR_CHECK(esp_mqtt_client_destroy(client));
    client = NULL;
    return ESP_OK;
}
