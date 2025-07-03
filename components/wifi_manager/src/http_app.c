/**
 * File: http_app.c
 * Description: HTTP server for captive-portal provisioning
 * Created on: 2025-06-25
 * Edited on:  2025-07-07 → v8.7.2 (refactored to serve all embedded assets)
 * Author: R. Andrew Ballard (c) 2025
 */

#include "http_app.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include <string.h>

extern const char _binary_src_index_html_start[] asm("_binary_src_index_html_start");
extern const char _binary_src_index_html_end[]   asm("_binary_src_index_html_end");
extern const char _binary_src_style_css_start[]  asm("_binary_src_style_css_start");
extern const char _binary_src_style_css_end[]    asm("_binary_src_style_css_end");
extern const char _binary_src_code_js_start[]    asm("_binary_src_code_js_start");
extern const char _binary_src_code_js_end[]      asm("_binary_src_code_js_end");
extern const char _binary_src_lock_svg_start[]   asm("_binary_src_lock_svg_start");
extern const char _binary_src_lock_svg_end[]     asm("_binary_src_lock_svg_end");
extern const char _binary_src_wifi0_svg_start[]  asm("_binary_src_wifi0_svg_start");
extern const char _binary_src_wifi0_svg_end[]    asm("_binary_src_wifi0_svg_end");
extern const char _binary_src_wifi1_svg_start[]  asm("_binary_src_wifi1_svg_start");
extern const char _binary_src_wifi1_svg_end[]    asm("_binary_src_wifi1_svg_end");
extern const char _binary_src_wifi2_svg_start[]  asm("_binary_src_wifi2_svg_start");
extern const char _binary_src_wifi2_svg_end[]    asm("_binary_src_wifi2_svg_end");
extern const char _binary_src_wifi3_svg_start[]  asm("_binary_src_wifi3_svg_start");
extern const char _binary_src_wifi3_svg_end[]    asm("_binary_src_wifi3_svg_end");

static const char *TAG = "HTTP_APP";
static httpd_handle_t server = NULL;

#define FILE_HANDLER(uri_path, mime_type, sym_start, sym_end)         \
    static esp_err_t handler_##sym_start(httpd_req_t *req) {          \
        size_t size = sym_end - sym_start;                            \
        httpd_resp_set_type(req, mime_type);                          \
        return httpd_resp_send(req, sym_start, size);                 \
    }

// Define individual asset handlers
FILE_HANDLER("/", "text/html", index_html_start, index_html_end)
FILE_HANDLER("/style.css", "text/css", style_css_start, style_css_end)
FILE_HANDLER("/code.js", "application/javascript", code_js_start, code_js_end)
FILE_HANDLER("/lock.svg", "image/svg+xml", lock_svg_start, lock_svg_end)
FILE_HANDLER("/wifi0.svg", "image/svg+xml", wifi0_svg_start, wifi0_svg_end)
FILE_HANDLER("/wifi1.svg", "image/svg+xml", wifi1_svg_start, wifi1_svg_end)
FILE_HANDLER("/wifi2.svg", "image/svg+xml", wifi2_svg_start, wifi2_svg_end)
FILE_HANDLER("/wifi3.svg", "image/svg+xml", wifi3_svg_start, wifi3_svg_end)

// POST /connect handler
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

    httpd_uri_t uris[] = {
        { .uri = "/",         .method = HTTP_GET,  .handler = handler_index_html_start, .user_ctx = NULL },
        { .uri = "/style.css",.method = HTTP_GET,  .handler = handler_style_css_start,  .user_ctx = NULL },
        { .uri = "/code.js",  .method = HTTP_GET,  .handler = handler_code_js_start,    .user_ctx = NULL },
        { .uri = "/lock.svg", .method = HTTP_GET,  .handler = handler_lock_svg_start,   .user_ctx = NULL },
        { .uri = "/wifi0.svg",.method = HTTP_GET,  .handler = handler_wifi0_svg_start,  .user_ctx = NULL },
        { .uri = "/wifi1.svg",.method = HTTP_GET,  .handler = handler_wifi1_svg_start,  .user_ctx = NULL },
        { .uri = "/wifi2.svg",.method = HTTP_GET,  .handler = handler_wifi2_svg_start,  .user_ctx = NULL },
        { .uri = "/wifi3.svg",.method = HTTP_GET,  .handler = handler_wifi3_svg_start,  .user_ctx = NULL },
        { .uri = "/connect",  .method = HTTP_POST, .handler = wifi_connect_handler,     .user_ctx = NULL }
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
