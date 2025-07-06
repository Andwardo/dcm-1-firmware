/*
 *  http_app.c
 *
 *  Created on: 2025-06-19
 *  Edited on: 2025-07-06 (local time)
 *      Author: Andwardo
 *      Version: v8.2.32
 */

#include <string.h>
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_wifi.h"
#include "wifi_manager.h"
#include "wifi_provisioning/wifi_config.h"

static const char *TAG = "HTTP_APP";

// Extern symbols from binary data
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[]   asm("_binary_style_css_end");

extern const uint8_t code_js_start[] asm("_binary_code_js_start");
extern const uint8_t code_js_end[]   asm("_binary_code_js_end");

extern const uint8_t lock_svg_start[] asm("_binary_lock_svg_start");
extern const uint8_t lock_svg_end[]   asm("_binary_lock_svg_end");

extern const uint8_t wifi0_svg_start[] asm("_binary_wifi0_svg_start");
extern const uint8_t wifi0_svg_end[]   asm("_binary_wifi0_svg_end");

extern const uint8_t wifi1_svg_start[] asm("_binary_wifi1_svg_start");
extern const uint8_t wifi1_svg_end[]   asm("_binary_wifi1_svg_end");

extern const uint8_t wifi2_svg_start[] asm("_binary_wifi2_svg_start");
extern const uint8_t wifi2_svg_end[]   asm("_binary_wifi2_svg_end");

extern const uint8_t wifi3_svg_start[] asm("_binary_wifi3_svg_start");
extern const uint8_t wifi3_svg_end[]   asm("_binary_wifi3_svg_end");

// Serve embedded static assets with correct content type and length
static esp_err_t serve_embedded_file(httpd_req_t *req, const uint8_t *start, const uint8_t *end, const char *content_type) {
    httpd_resp_set_type(req, content_type);
    size_t length = end - start;

    // Hotfix: if serving a text asset, exclude null byte
    if (strstr(content_type, "text/") == content_type || strstr(content_type, "application/javascript") == content_type) {
        if (length > 0 && start[length - 1] == '\0') {
            length--;  // Trim null byte
        }
    }

    return httpd_resp_send(req, (const char *)start, length);
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Serving / or /index.html (%d bytes)", (int)(index_html_end - index_html_start));
    return serve_embedded_file(req, index_html_start, index_html_end, "text/html");
}

static esp_err_t style_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Serving /style.css (%d bytes)", (int)(style_css_end - style_css_start));
    return serve_embedded_file(req, style_css_start, style_css_end, "text/css");
}

static esp_err_t js_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Serving /code.js (%d bytes)", (int)(code_js_end - code_js_start));
    return serve_embedded_file(req, code_js_start, code_js_end, "application/javascript");
}

static esp_err_t svg_get_handler(httpd_req_t *req, const uint8_t *start, const uint8_t *end, const char *name) {
    ESP_LOGI(TAG, "Serving /%s (%d bytes)", name, (int)(end - start));
    return serve_embedded_file(req, start, end, "image/svg+xml");
}

#define DEFINE_SVG_HANDLER(uri, start, end, name)                     \
    static esp_err_t name##_get_handler(httpd_req_t *req) {          \
        return svg_get_handler(req, start, end, #name ".svg");       \
    }

DEFINE_SVG_HANDLER("/lock.svg",   lock_svg_start,   lock_svg_end,   lock)
DEFINE_SVG_HANDLER("/wifi0.svg",  wifi0_svg_start,  wifi0_svg_end,  wifi0)
DEFINE_SVG_HANDLER("/wifi1.svg",  wifi1_svg_start,  wifi1_svg_end,  wifi1)
DEFINE_SVG_HANDLER("/wifi2.svg",  wifi2_svg_start,  wifi2_svg_end,  wifi2)
DEFINE_SVG_HANDLER("/wifi3.svg",  wifi3_svg_start,  wifi3_svg_end,  wifi3)

httpd_handle_t start_webserver(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP server (provisioning mode: %s)...",
             wifi_manager_get_provisioning_status() ? "ON" : "OFF");

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t root = {
            .uri       = "/",
            .method    = HTTP_GET,
            .handler   = root_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &root);

        httpd_uri_t style = {
            .uri       = "/style.css",
            .method    = HTTP_GET,
            .handler   = style_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &style);

        httpd_uri_t code = {
            .uri       = "/code.js",
            .method    = HTTP_GET,
            .handler   = js_get_handler,
            .user_ctx  = NULL
        };
        httpd_register_uri_handler(server, &code);

        httpd_register_uri_handler(server, &(httpd_uri_t) {
            .uri = "/lock.svg", .method = HTTP_GET, .handler = lock_get_handler });
        httpd_register_uri_handler(server, &(httpd_uri_t) {
            .uri = "/wifi0.svg", .method = HTTP_GET, .handler = wifi0_get_handler });
        httpd_register_uri_handler(server, &(httpd_uri_t) {
            .uri = "/wifi1.svg", .method = HTTP_GET, .handler = wifi1_get_handler });
        httpd_register_uri_handler(server, &(httpd_uri_t) {
            .uri = "/wifi2.svg", .method = HTTP_GET, .handler = wifi2_get_handler });
        httpd_register_uri_handler(server, &(httpd_uri_t) {
            .uri = "/wifi3.svg", .method = HTTP_GET, .handler = wifi3_get_handler });

        ESP_LOGI(TAG, "HTTP server started successfully");
        return server;
    }

    ESP_LOGE(TAG, "Failed to start HTTP server");
    return NULL;
}