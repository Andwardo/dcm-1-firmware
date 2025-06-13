/*
 * File: components/mqtt_manager/mqtt_manager.c
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 10:00:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "mqtt_manager.h"
#include "certs.h"
#include "esp_log.h"
#include "esp_event.h"
#include "mqtt_client.h"
#include "freertos/event_groups.h"

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t s_client = NULL;
static EventGroupHandle_t s_mqtt_event_group = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base,
                               int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            xEventGroupSetBits(s_mqtt_event_group, MQTT_CONNECTED_BIT);
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        default:
            break;
    }
}

void mqtt_manager_init(void)
{
    s_mqtt_event_group = xEventGroupCreate();

    const esp_mqtt_client_config_t cfg = {
        .uri = CONFIG_MQTT_URI,
        .cert_pem = (const char *)server_cert_pem_start,
    };
    s_client = esp_mqtt_client_init(&cfg);
    esp_mqtt_client_register_event(s_client, ESP_EVENT_ANY_ID,
                                   mqtt_event_handler, NULL);
}

void mqtt_manager_start(void)
{
    if (s_client) {
        esp_mqtt_client_start(s_client);
    }
}

EventGroupHandle_t mqtt_event_group_handle(void)
{
    return s_mqtt_event_group;
}

esp_err_t mqtt_manager_publish_sensor_state(bool power, bool water, bool pads)
{
    if (!s_client) {
        return ESP_ERR_INVALID_STATE;
    }
    char payload[128];
    int len = snprintf(payload, sizeof(payload),
                       "{\"power\":%s,\"water\":%s,\"pads\":%s}",
                       power ? "true" : "false",
                       water ? "true" : "false",
                       pads ? "true" : "false");
    if (len < 0) {
        return ESP_FAIL;
    }
    int msg_id = esp_mqtt_client_publish(
        s_client, CONFIG_MQTT_TOPIC, payload, len, 1, 0
    );
    ESP_LOGI(TAG, "Published msg_id=%d payload=%s", msg_id, payload);
    return msg_id >= 0 ? ESP_OK : ESP_FAIL;
}
