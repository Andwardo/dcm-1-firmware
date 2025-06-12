/*
 * File: components/mqtt_manager/mqtt_manager.c
 *
 * Created on: 12 June 2025 09:10:00
 * Last edited on: 12 June 2025 09:10:00
 *
 * Version: 7.5.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "mqtt_manager.h"
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"

static const char *TAG = "MQTT_MANAGER";

static void log_error_if_nonzero(const char *message, int error_code)
{
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Successfully connected to mqtts://dev1.pgapi.net:8883");
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            log_error_if_nonzero("reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            log_error_if_nonzero("reported from tls stack", event->error_handle->esp_tls_stack_err);
            log_error_if_nonzero("captured as transport's socket errno",  event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGD(TAG, "Other MQTT event id:%d", (int)event->event_id);
        break;
    }
}

void mqtt_manager_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://dev1.pgapi.net:8883",
    };

    ESP_LOGI(TAG, "Starting MQTT client...");
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));
    ESP_ERROR_CHECK(esp_mqtt_client_start(client));
}
