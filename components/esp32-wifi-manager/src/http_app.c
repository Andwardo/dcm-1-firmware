/**
 * File: http_app.c
 * Description: HTTP server for captive-portal provisioning
 * Created on: 2025-06-25
 * Edited on:  2025-07-07  ← new
 * Version: v8.7.0
 * Author: R. Andrew Ballard (c) 2025
 */

#include "http_app.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include <string.h>

// Embed your index.html and style.css as C strings or load from SPIFFS here.
// For simplicity, assume they’re compiled in as extern const char *…
extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char style_css_start[]  asm("_binary_style_css_start");
extern const char style_css_end[]    asm("_binary_style_css_end");

static const char *TAG = "HTTP_APP";
static httpd_handle_t server = NULL;

// Serve the root page
static esp_err_t root_get_handler(httpd_req_t *req) {
    size_t size = index_html_end - index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, index_html_start, size);
    return ESP_OK;
}

// Serve style.css
static esp_err_t style_get_handler(httpd_req_t *req) {
    size_t size = style_css_end - style_css_start;
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, style_css_start, size);
    return ESP_OK;
}

esp_err_t wifi_connect_handler(httpd_req_t *req);  // your existing POST handler

void http_app_start(bool lru_purge_enable) {
    if (server) {
        ESP_LOGW(TAG, "HTTP server already running");
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;  // allow wildcard URIs

    ESP_LOGI(TAG, "Starting HTTP server...");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    // Register GET /
    httpd_uri_t root_uri = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = root_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &root_uri);

    // Register GET /style.css
    httpd_uri_t style_uri = {
        .uri       = "/style.css",
        .method    = HTTP_GET,
        .handler   = style_get_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &style_uri);

    // Register your POST /connect
    httpd_uri_t connect_uri = {
        .uri       = "/connect",
        .method    = HTTP_POST,
        .handler   = wifi_connect_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &connect_uri);

    ESP_LOGI(TAG, "HTTP captive-portal ready");
}

void http_app_stop() {
    if (!server) return;
    ESP_LOGI(TAG, "Stopping HTTP server...");
    httpd_stop(server);
    server = NULL;
}
