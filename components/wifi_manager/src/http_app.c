/*
 *  http_app.c
 *
 *  Created on: 2025-06-12
 *  Edited on: 2025-07-07
 *      Author: Andwardo
 *      Version: v8.2.38
 */

#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "http_app.h"

extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");
extern const uint8_t code_js_start[]    asm("_binary_code_js_start");
extern const uint8_t code_js_end[]      asm("_binary_code_js_end");
extern const uint8_t style_css_start[]  asm("_binary_style_css_start");
extern const uint8_t style_css_end[]    asm("_binary_style_css_end");

static const char *TAG = "http_app";

static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start);
}

static esp_err_t js_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "application/javascript");
    return httpd_resp_send(req, (const char *)code_js_start, code_js_end - code_js_start);
}

static esp_err_t css_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/css");
    return httpd_resp_send(req, (const char *)style_css_start, style_css_end - style_css_start);
}

httpd_handle_t start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t index_uri = {
            .uri      = "/",
            .method   = HTTP_GET,
            .handler  = index_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &index_uri);

        httpd_uri_t js_uri = {
            .uri      = "/code.js",
            .method   = HTTP_GET,
            .handler  = js_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &js_uri);

        httpd_uri_t css_uri = {
            .uri      = "/style.css",
            .method   = HTTP_GET,
            .handler  = css_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &css_uri);

        ESP_LOGI(TAG, "HTTP server started");
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server");
    }

    return server;
}
