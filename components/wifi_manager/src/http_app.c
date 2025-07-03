/**
 * File: http_app.c
 * Description: HTTP server for captive-portal provisioning
 * Created on: 2025-06-25
 * Edited on:  2025-07-07 → v8.7.1 (fixed missing POST handler)
 * Author: R. Andrew Ballard (c) 2025
 */

#include "http_app.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include <string.h>

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char style_css_start[]  asm("_binary_style_css_start");
extern const char style_css_end[]    asm("_binary_style_css_end");

static const char *TAG = "HTTP_APP";
static httpd_handle_t server = NULL;

// GET /
static esp_err_t root_get_handler(httpd_req_t *req) {
    size_t size = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html_start, size);
}

// GET /style.css
static esp_err_t style_get_handler(httpd_req_t *req) {
    size_t size = style_css_end - style_css_start;
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, style_css_start, size);
}

// POST /connect
// Full handler implementation
esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char ssid[33]     = {0};
    char password[65] = {0};

    // Read SSID
    int ret = httpd_req_recv(req, ssid, sizeof(ssid) - 1);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive SSID");
        return ESP_FAIL;
    }

    // Read password
    ret = httpd_req_recv(req, password, sizeof(password) - 1);
    if (ret <= 0) {
        ESP_LOGE(TAG, "Failed to receive password");
        return ESP_FAIL;
    }

    // Enqueue connect request
    wifi_manager_message_t msg = {
        .msg_id = WIFI_MANAGER_MSG_CONNECT_STA
    };
    strlcpy((char*)msg.sta_config.sta.ssid,     ssid,     sizeof(msg.sta_config.sta.ssid));
    strlcpy((char*)msg.sta_config.sta.password, password, sizeof(msg.sta_config.sta.password));

    if (wifi_manager_send_message(&msg) != pdTRUE) {
        ESP_LOGE(TAG, "Failed to queue Wi-Fi connect message");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Queued Wi-Fi connect request for \"%s\"", ssid);
    httpd_resp_sendstr(req, "OK");
    return ESP_OK;
}

void http_app_start(bool lru_purge_enable) {
    if (server) {
        ESP_LOGW(TAG, "HTTP server already running");
        return;
    }
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    ESP_LOGI(TAG, "Starting HTTP server...");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    // Register URI handlers
    httpd_uri_t uris[] = {
        { .uri = "/",         .method = HTTP_GET,  .handler = root_get_handler,   .user_ctx = NULL },
        { .uri = "/style.css",.method = HTTP_GET,  .handler = style_get_handler,  .user_ctx = NULL },
        { .uri = "/connect",  .method = HTTP_POST, .handler = wifi_connect_handler, .user_ctx = NULL }
    };
    for (size_t i = 0; i < sizeof(uris)/sizeof(uris[0]); i++) {
        httpd_register_uri_handler(server, &uris[i]);
    }
    ESP_LOGI(TAG, "HTTP captive-portal ready");
}

void http_app_stop(void) {
    if (!server) {
        return;
    }
    ESP_LOGI(TAG, "Stopping HTTP server...");
    httpd_stop(server);
    server = NULL;
}
