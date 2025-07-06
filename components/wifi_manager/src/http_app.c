/*
 * http_app.c
 *
 * Created on: 2025-06-18
 * Edited on: 2025-07-06
 *     Author: R. Andrew Ballard
 *     Version: v8.2.34
 */

#include "http_app.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_vfs.h"
#include <string.h>

// Extern symbols for embedded files
extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char style_css_start[] asm("_binary_style_css_start");
extern const char style_css_end[]   asm("_binary_style_css_end");
extern const char code_js_start[]   asm("_binary_code_js_start");
extern const char code_js_end[]     asm("_binary_code_js_end");

static const char *TAG = "HTTP_APP";
static httpd_handle_t server = NULL;

static esp_err_t index_handler(httpd_req_t *req) {
    const size_t index_html_len = index_html_end - index_html_start;
    ESP_LOGI(TAG, "Serving /index.html (%u bytes)", (unsigned)index_html_len);
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html_start, index_html_len);
}

static esp_err_t style_handler(httpd_req_t *req) {
    const size_t style_len = style_css_end - style_css_start;
    ESP_LOGI(TAG, "Serving /style.css (%u bytes)", (unsigned)style_len);
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, style_css_start, style_len);
}

static esp_err_t script_handler(httpd_req_t *req) {
    const size_t script_len = code_js_end - code_js_start;
    ESP_LOGI(TAG, "Serving /code.js (%u bytes)", (unsigned)script_len);
    httpd_resp_set_type(req, "application/javascript");
    return httpd_resp_send(req, code_js_start, script_len);
}

void http_app_start(bool is_provisioning_mode) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP server (provisioning mode: %s)...", is_provisioning_mode ? "ON" : "OFF");

    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    httpd_uri_t uri_index = {
        .uri       = "/",
        .method    = HTTP_GET,
        .handler   = index_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_index);

    httpd_uri_t uri_style = {
        .uri       = "/style.css",
        .method    = HTTP_GET,
        .handler   = style_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_style);

    httpd_uri_t uri_script = {
        .uri       = "/code.js",
        .method    = HTTP_GET,
        .handler   = script_handler,
        .user_ctx  = NULL
    };
    httpd_register_uri_handler(server, &uri_script);

    ESP_LOGI(TAG, "HTTP server started successfully");
}
