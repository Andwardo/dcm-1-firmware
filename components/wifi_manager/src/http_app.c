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
#include "esp_log.h"
#include <string.h>
#include <stdlib.h>

#define TAG "HTTP_APP"

static esp_err_t (*handler_hook)(httpd_req_t *req) = NULL;

esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char content[128];
    int ret, remaining = req->content_len;

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, content, remaining < sizeof(content) ? remaining : sizeof(content))) <= 0) {
            return ESP_FAIL;
        }
        remaining -= ret;
    }

    // Simple example: parse "ssid=xxx&password=yyy"
    char *ssid = strstr(content, "ssid=");
    char *pass = strstr(content, "password=");
    if (ssid && pass) {
        ssid += 5;
        char *amp = strchr(ssid, '&');
        if (amp) *amp = '\0';
        pass += 9;

        wifi_manager_connect_sta(ssid, pass);
        return httpd_resp_sendstr(req, "Connecting...");
    }

    return ESP_FAIL;
}

esp_err_t http_app_set_handler_hook(httpd_method_t method, esp_err_t (*handler)(httpd_req_t *req)) {
    if (method != HTTP_POST) {
        return ESP_ERR_INVALID_ARG;
    }
    handler_hook = handler;
    return ESP_OK;
}

static esp_err_t post_connect_handler(httpd_req_t *req) {
    if (handler_hook) {
        return handler_hook(req);
    }
    return wifi_connect_handler(req);
}

void http_app_start(bool lru_purge_enable) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = lru_purge_enable;

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t connect_uri = {
            .uri      = "/connect",
            .method   = HTTP_POST,
            .handler  = post_connect_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &connect_uri);
        ESP_LOGI(TAG, "HTTP server started.");
    }
}

void http_app_stop(void) {
    // Implement later if needed.
}
