/**
 * File: mqtt_manager.c
 * Description: MQTT client manager for PianoGuard DCM-1
 * Created on: 2025-06-20
 * Edited on:  2025-07-02
 * Version: v8.6.12
 * Author: R. Andrew Ballard (c) 2025
 */

#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

// Embedded certificate data (generated via xxd -i)
#include "root_ca.h"    // defines unsigned char root_ca_pem[] and unsigned int root_ca_pem_len
#include "client_crt.h" // defines unsigned char client_crt[] and unsigned int client_crt_len
#include "client_key.h" // defines unsigned char client_key[] and unsigned int client_key_len

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t client = NULL;

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    ESP_LOGD(TAG, "MQTT event id: %" PRIi32, event_id);
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle) {
                ESP_LOGE(TAG, "esp-tls error: 0x%x", event->error_handle->esp_tls_last_esp_err);
                ESP_LOGE(TAG, "TLS stack error: 0x%x", event->error_handle->esp_tls_stack_err);
                ESP_LOGE(TAG, "Errno: %d (%s)", event->error_handle->esp_transport_sock_errno,
                         strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            break;
    }
}

void mqtt_manager_init(void) {
    ESP_LOGI(TAG, "Initializing MQTT with embedded certificates...");

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri                         = "mqtts://dev1.pgapi.net:8883",
        .broker.verification.certificate            = (const char *)root_ca_pem,
        .broker.verification.certificate_len        = root_ca_pem_len,
        .credentials.authentication.certificate     = (const char *)client_crt,
        .credentials.authentication.certificate_len = client_crt_len,
        .credentials.authentication.key             = (const char *)client_key,
        .credentials.authentication.key_len         = client_key_len,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize MQTT client");
        return;
    }

    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT client started");
}
