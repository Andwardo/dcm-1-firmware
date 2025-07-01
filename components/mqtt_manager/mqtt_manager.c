/**
 * File: mqtt_manager.c
 * Description: MQTT client manager for PianoGuard DCM-1
 * Created on: 2025-06-20
 * Edited on:  2025-07-01
 * Version: v8.6.5
 * Author: R. Andrew Ballard (c) 2025
 */

#include "mqtt_manager.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_spiffs.h"
#include "esp_err.h"
#include "esp_tls.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static const char *TAG = "MQTT_MANAGER";
static esp_mqtt_client_handle_t client = NULL;

#define ROOT_CA_PATH   "/spiffs/root_ca.pem"
#define CLIENT_CRT_PATH "/spiffs/client.crt"
#define CLIENT_KEY_PATH "/spiffs/client.key"

static char *load_file(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file: %s", path);
        return NULL;
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);

    char *buf = malloc(len + 1);
    if (!buf) {
        ESP_LOGE(TAG, "Memory allocation failed for file: %s", path);
        fclose(f);
        return NULL;
    }

    size_t read = fread(buf, 1, len, f);
    buf[read] = '\0';
    fclose(f);
    return buf;
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
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
                ESP_LOGE(TAG, "esp-tls err: 0x%x", event->error_handle->esp_tls_last_esp_err);
                ESP_LOGE(TAG, "TLS stack err: 0x%x", event->error_handle->esp_tls_stack_err);
                ESP_LOGE(TAG, "Errno: %d (%s)", event->error_handle->esp_transport_sock_errno,
                         strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;
        default:
            break;
    }
}

void mqtt_manager_init(void)
{
    ESP_LOGI(TAG, "Loading MQTT certs from SPIFFS...");

    char *root_ca = load_file(ROOT_CA_PATH);
    char *client_crt = load_file(CLIENT_CRT_PATH);
    char *client_key = load_file(CLIENT_KEY_PATH);

    if (!root_ca || !client_crt || !client_key) {
        ESP_LOGE(TAG, "Failed to load certificates from SPIFFS. MQTT will not start.");
        free(root_ca);
        free(client_crt);
        free(client_key);
        return;
    }

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "mqtts://dev1.pgapi.net:8883",
        .cert_pem = root_ca,
        .client_cert_pem = client_crt,
        .client_key_pem = client_key
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);

    // Free certs after client copies them (ESP MQTT stores them internally)
    free(root_ca);
    free(client_crt);
    free(client_key);
}
