/*
 * http_app.c
 *
 * Created on: 2025-06-18
 * Edited on: 2025-07-06
 *     Author: R. Andrew Ballard
 *     Version: v8.2.32
 */

#include "http_app.h"
#include "wifi_manager.h"

#include <esp_log.h>
#include <esp_http_server.h>
#include <string.h>
#include <sys/param.h>

#define TAG "HTTP_APP"

static httpd_handle_t server = NULL;

esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char content[128];
    int remaining = req->content_len;
    int ret;

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, content, MIN(remaining, sizeof(content)))) <= 0) {
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    content[req->content_len] = '\0';

    char ssid[32] = {0};
    char pass[64] = {0};

    sscanf(content, "ssid=%31[^&]&password=%63s", ssid, pass);
    ESP_LOGI(TAG, "Received credentials: SSID=%s", ssid);

    wifi_manager_connect_sta(ssid, pass);
    httpd_resp_sendstr(req, "Connecting...");

    return ESP_OK;
}

esp_err_t http_app_set_handler_hook(httpd_method_t method,
                                    esp_err_t (*handler)(httpd_req_t *req)) {
    return ESP_ERR_NOT_SUPPORTED;
}

void http_app_start(bool lru_purge_enable) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = lru_purge_enable;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t connect_uri = {
            .uri      = "/connect",
            .method   = HTTP_POST,
            .handler  = wifi_connect_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &connect_uri);
    }
}

void http_app_stop(void) {
    if (server) {
        httpd_stop(server);
        server = NULL;
    }
}
