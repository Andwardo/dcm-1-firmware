/**
 * File: http_app.c
 * Description: HTTP server handlers for Wi-Fi provisioning (Captive Portal)
 * Created on: 2025-06-25
 * Edited on:  2025-07-01
 * Version: v8.6.7
 * Author: R. Andrew Ballard (c) 2025
 */



#include <stdio.h>
#include <string.h>
#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_wifi.h"

static const char *TAG = "HTTP_APP";

/* Example handler for connecting to a Wi-Fi network via POST */
esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char ssid[33] = {0};
    char password[65] = {0};

    /* Parse form data (simple parsing, assumes exact keys/format) */
    int ret = httpd_req_recv(req, ssid, sizeof(ssid) - 1);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive SSID");
        return ESP_FAIL;
    }

    ret = httpd_req_recv(req, password, sizeof(password) - 1);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive password");
        return ESP_FAIL;
    }

    wifi_manager_message_t msg = {0};
    memset(&msg.sta_config, 0, sizeof(wifi_config_t));
    strlcpy((char *)msg.sta_config.sta.ssid, ssid, sizeof(msg.sta_config.sta.ssid));
    strlcpy((char *)msg.sta_config.sta.password, password, sizeof(msg.sta_config.sta.password));
    msg.msg_id = WIFI_MANAGER_MSG_CONNECT_STA;

    if (wifi_manager_send_message(&msg) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue Wi-Fi connect message");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Wi-Fi connect message sent successfully");
    httpd_resp_sendstr(req, "Wi-Fi connection initiated");
    return ESP_OK;
}
