/**
 * File: mqtt_manager.c
 * Description: MQTT client manager for PianoGuard DCM-1
 * Created on: 2025-06-20
 * Edited on:  2025-07-01
 * Version: v8.6.8
 * Author: R. Andrew Ballard (c) 2025
 */

#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_spiffs.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t client = NULL;

static char *read_cert_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open cert file: %s", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = calloc(1, size + 1);
    if (!buffer) {
        ESP_LOGE(TAG, "Failed to allocate memory for cert: %s", path);
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    fclose(f);
    return buffer;
}

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
    ESP_LOGI(TAG, "Mounting SPIFFS for certificate loading...");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 5,
        .format_if_mount_failed = true
    };
    ESP_ERROR_CHECK(esp_vfs_spiffs_register(&conf));

    char *root_ca    = read_cert_file("/spiffs/root_ca.pem");
    char *client_crt = read_cert_file("/spiffs/client.crt");
    char *client_key = read_cert_file("/spiffs/client.key");

    if (!root_ca || !client_crt || !client_key) {
        ESP_LOGE(TAG, "One or more certificates failed to load. MQTT init aborted.");
        return;
    }

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://dev1.pgapi.net:8883",
        .cert_pem = root_ca,
        .client_cert_pem = client_crt,
        .client_key_pem = client_key
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "MQTT client started.");
}
