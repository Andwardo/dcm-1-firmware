/**
 *  http_app.c
 *
 *  Created on: 2025-06-25
 *  Edited on:  2025-07-06 (local time)
 *      Author: R. Andrew Ballard
 *      Version: v8.7.8
 */

#include "http_app.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "string.h"

// Embedded assets
extern const uint8_t _binary_index_html_start[]   asm("_binary_index_html_start");
extern const uint8_t _binary_index_html_end[]     asm("_binary_index_html_end");
extern const uint8_t _binary_code_js_start[]      asm("_binary_code_js_start");
extern const uint8_t _binary_code_js_end[]        asm("_binary_code_js_end");
extern const uint8_t _binary_style_css_start[]    asm("_binary_style_css_start");
extern const uint8_t _binary_style_css_end[]      asm("_binary_style_css_end");

static const char *TAG = "HTTP_APP";

esp_err_t index_handler(httpd_req_t *req) {
    const size_t index_html_len = _binary_index_html_end - _binary_index_html_start;
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)_binary_index_html_start, index_html_len);
    return ESP_OK;
}

esp_err_t js_handler(httpd_req_t *req) {
    const size_t js_len = _binary_code_js_end - _binary_code_js_start;
    httpd_resp_set_type(req, "application/javascript");
    httpd_resp_send(req, (const char *)_binary_code_js_start, js_len);
    return ESP_OK;
}

esp_err_t css_handler(httpd_req_t *req) {
    const size_t css_len = _binary_style_css_end - _binary_style_css_start;
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)_binary_style_css_start, css_len);
    return ESP_OK;
}

void http_app_start() {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 8;

    ESP_LOGI(TAG, "Starting HTTP server...");

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = index_handler
        };
        httpd_uri_t js_uri = {
            .uri = "/code.js",
            .method = HTTP_GET,
            .handler = js_handler
        };
        httpd_uri_t css_uri = {
            .uri = "/style.css",
            .method = HTTP_GET,
            .handler = css_handler
        };

        httpd_register_uri_handler(server, &index_uri);
        httpd_register_uri_handler(server, &js_uri);
        httpd_register_uri_handler(server, &css_uri);
    }
}
