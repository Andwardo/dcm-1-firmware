/*
 * File: components/esp32-wifi-manager/http_app.c
 *
 * Created on: 2025-06-13 10:30:00
 * Edited on:  2025-06-15 08:47:00
 *
 * Version: 8.1.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "http_app.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "cJSON.h"
#include "nvs_flash.h"
#include "nvs.h"

static const char *TAG = "HTTP_APP";
static httpd_handle_t server = NULL;

/**
 * @brief Scan handler: /api/wifi-scan
 */
static esp_err_t scan_handler(httpd_req_t *req)
{
    ESP_LOGI(TAG, "Handling Wi-Fi scan request...");

    wifi_scan_config_t scan_config = {
        .ssid = 0,
        .bssid = 0,
        .channel = 0,
        .show_hidden = true,
        .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };

    esp_wifi_scan_start(&scan_config, true);

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);

    wifi_ap_record_t *ap_list = calloc(ap_count, sizeof(wifi_ap_record_t));
    esp_wifi_scan_get_ap_records(&ap_count, ap_list);

    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < ap_count; i++) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddStringToObject(item, "ssid", (char *)ap_list[i].ssid);
        cJSON_AddNumberToObject(item, "rssi", ap_list[i].rssi);
        cJSON_AddBoolToObject(item, "auth", ap_list[i].authmode != WIFI_AUTH_OPEN);
        cJSON_AddItemToArray(root, item);
    }

    char *json = cJSON_PrintUnformatted(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json, strlen(json));

    free(ap_list);
    cJSON_Delete(root);
    free(json);

    return ESP_OK;
}

/**
 * @brief Connect handler: /api/connect
 */
static esp_err_t connect_handler(httpd_req_t *req)
{
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf));
    if (ret <= 0) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Failed to read POST data");
        return ESP_FAIL;
    }

    cJSON *json = cJSON_ParseWithLength(buf, ret);
    if (!json) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    const cJSON *ssid = cJSON_GetObjectItem(json, "ssid");
    const cJSON *password = cJSON_GetObjectItem(json, "password");

    if (!cJSON_IsString(ssid) || !cJSON_IsString(password)) {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing SSID or password");
        return ESP_FAIL;
    }

    // Save to NVS
    nvs_handle_t nvs;
    esp_err_t err = nvs_open("wifi_creds", NVS_READWRITE, &nvs);
    if (err != ESP_OK) {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "NVS open failed");
        return ESP_FAIL;
    }

    nvs_set_str(nvs, "ssid", ssid->valuestring);
    nvs_set_str(nvs, "pass", password->valuestring);
    nvs_commit(nvs);
    nvs_close(nvs);
    cJSON_Delete(json);

    ESP_LOGI(TAG, "Saved Wi-Fi credentials to NVS");

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":true}");

    vTaskDelay(pdMS_TO_TICKS(1500));
    esp_restart();

    return ESP_OK;
}

/**
 * @brief Register all HTTP routes
 */
static void register_routes(httpd_handle_t server)
{
    httpd_uri_t scan_uri = {
        .uri = "/api/wifi-scan",
        .method = HTTP_GET,
        .handler = scan_handler,
        .user_ctx = NULL,
    };

    httpd_uri_t connect_uri = {
        .uri = "/api/connect",
        .method = HTTP_POST,
        .handler = connect_handler,
        .user_ctx = NULL,
    };

    httpd_register_uri_handler(server, &scan_uri);
    httpd_register_uri_handler(server, &connect_uri);
}

/**
 * @brief Start embedded HTTP server
 */
void http_app_start(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP server...");
    if (httpd_start(&server, &config) == ESP_OK) {
        register_routes(server);
        ESP_LOGI(TAG, "HTTP server started successfully");
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }
}
