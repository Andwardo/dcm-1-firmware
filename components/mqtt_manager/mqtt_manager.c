/**
 * File: mqtt_manager.h
 * Description: MQTT client interface with certificate loading and callbacks.
 * Created on: 2025-06-15
 * Edited on:  2025-07-1
 * Version: v8.5.7
 * Author: R. Andrew Ballard (c) 2025
 * Added include for inttypes
 */

#include <inttypes.h>

#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include "mqtt_client.h"

typedef void (*mqtt_data_callback_t)(esp_mqtt_event_handle_t event);
typedef void (*mqtt_error_callback_t)(esp_mqtt_error_codes_t *error_handle);

void start_mqtt_manager(void);
void mqtt_manager_publish(const char* topic, const char* data);
void mqtt_manager_register_data_callback(mqtt_data_callback_t callback);
void mqtt_manager_register_error_callback(mqtt_error_callback_t callback);

#endif // MQTT_MANAGER_H
/**
 * File: mqtt_manager.c
 * Description: Manages the secure MQTT connection using mTLS.
 * Created on: 2025-06-15
 * Edited on:  2025-07-01
 * Version: v8.6.4
 * Author: R. Andrew Ballard (c) 2025
 */

#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t mqtt_client = NULL;
static mqtt_data_callback_t data_callback = NULL;
static mqtt_error_callback_t error_callback = NULL;

static void log_error_if_nonzero(const char *message, int error_code) {
    if (error_code != 0) {
        ESP_LOGE(TAG, "Last error %s: 0x%x", message, error_code);
    }
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED: Connected to mqtts://dev1.pgapi.net:8883");
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "MQTT_EVENT_DISCONNECTED");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
            if (event->error_handle && event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                log_error_if_nonzero("esp-tls", event->error_handle->esp_tls_last_esp_err);
                log_error_if_nonzero("tls stack", event->error_handle->esp_tls_stack_err);
                if (error_callback) {
                    error_callback(event->error_handle);
                }
            }
            break;
        case MQTT_EVENT_DATA:
            if (data_callback) {
                data_callback(event);
            }
            break;
        default:
            ESP_LOGD(TAG, "Unhandled MQTT event: %" PRIi32, event_id);
            break;
    }
}

void start_mqtt_manager(void) {
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtts://dev1.pgapi.net:8883",
        .cert = {
            .cert_pem = NULL,
            .use_global_ca_store = false,
            .client_cert_pem = NULL,
            .client_key_pem = NULL
        },
        .credentials = {
            .authentication = {
                .certificate = {
                    .cert_pem = (const char *)"certs/client.crt",
                    .key_pem  = (const char *)"certs/client.key",
                    .cert_key_password = NULL
                }
            }
        },
        .broker.verification = {
            .verify_certificate = true,
            .use_global_ca_store = false,
            .crt_bundle_attach = NULL,
            .cert_pem = (const char *)"certs/root_ca.pem"
        },
        .network = {
            .reconnect_timeout_ms = 10000
        }
    };const esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = "mqtts://dev1.pgapi.net:8883",
    .broker.verification.certificate = (const char *)"certs/root_ca.pem",
    .credentials = {
        .authentication = {
            .certificate = {
                .cert_pem = (const char *)"certs/client.crt",
                .key_pem  = (const char *)"certs/client.key",
                .cert_key_password = NULL
            }
        }
    }
};

    ESP_LOGI(TAG, "Initializing MQTT client with mTLS support...");
    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(mqtt_client);
}

void mqtt_manager_publish(const char* topic, const char* data) {
    if (!mqtt_client) {
        ESP_LOGW(TAG, "MQTT client not started. Cannot publish.");
        return;
    }
    esp_mqtt_client_publish(mqtt_client, topic, data, 0, 1, 0);
}

void mqtt_manager_register_data_callback(mqtt_data_callback_t callback) {
    data_callback = callback;
}

void mqtt_manager_register_error_callback(mqtt_error_callback_t callback) {
    error_callback = callback;
}
