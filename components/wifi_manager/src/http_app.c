/*
 *  http_app.c
 *
 *  Created on: 2025-07-06
 *  Edited on: 2025-07-06 (local time)
 *      Author: Andwardo
 *      Version: v8.7.11
 */

#include "http_app.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_server.h"
#include "esp_vfs.h"

#define TAG "HTTP_APP"

static httpd_handle_t server = NULL;

extern const uint8_t index_html_start[]   asm("_binary_index_html_start");
extern const uint8_t index_html_end[]     asm("_binary_index_html_end");
extern const uint8_t style_css_start[]    asm("_binary_style_css_start");
extern const uint8_t style_css_end[]      asm("_binary_style_css_end");
extern const uint8_t code_js_start[]      asm("_binary_code_js_start");
extern const uint8_t code_js_end[]        asm("_binary_code_js_end");
extern const uint8_t lock_svg_start[]     asm("_binary_lock_svg_start");
extern const uint8_t lock_svg_end[]       asm("_binary_lock_svg_end");
extern const uint8_t wifi0_svg_start[]    asm("_binary_wifi0_svg_start");
extern const uint8_t wifi0_svg_end[]      asm("_binary_wifi0_svg_end");
extern const uint8_t wifi1_svg_start[]    asm("_binary_wifi1_svg_start");
extern const uint8_t wifi1_svg_end[]      asm("_binary_wifi1_svg_end");
extern const uint8_t wifi2_svg_start[]    asm("_binary_wifi2_svg_start");
extern const uint8_t wifi2_svg_end[]      asm("_binary_wifi2_svg_end");
extern const uint8_t wifi3_svg_start[]    asm("_binary_wifi3_svg_start");
extern const uint8_t wifi3_svg_end[]      asm("_binary_wifi3_svg_end");

#define DEFINE_URI_HANDLER(uri_path, mime_type, content_start, content_end)              \
    static esp_err_t handler_##content_start(httpd_req_t *req) {                         \
        httpd_resp_set_type(req, mime_type);                                             \
        httpd_resp_send(req, (const char *)content_start,                               \
                        content_end - content_start);                                    \
        return ESP_OK;                                                                   \
    }

#define URI_HANDLER_ENTRY(uri_path, content_start) {                                     \
    .uri = uri_path,                                                                     \
    .method = HTTP_GET,                                                                  \
    .handler = handler_##content_start,                                                  \
    .user_ctx = NULL                                                                     \
}

DEFINE_URI_HANDLER("/",         "text/html",  index_html_start, index_html_end);
DEFINE_URI_HANDLER("/index.html", "text/html",  index_html_start, index_html_end);
DEFINE_URI_HANDLER("/style.css", "text/css",   style_css_start, style_css_end);
DEFINE_URI_HANDLER("/code.js",  "application/javascript", code_js_start, code_js_end);
DEFINE_URI_HANDLER("/lock.svg", "image/svg+xml", lock_svg_start, lock_svg_end);
DEFINE_URI_HANDLER("/wifi0.svg", "image/svg+xml", wifi0_svg_start, wifi0_svg_end);
DEFINE_URI_HANDLER("/wifi1.svg", "image/svg+xml", wifi1_svg_start, wifi1_svg_end);
DEFINE_URI_HANDLER("/wifi2.svg", "image/svg+xml", wifi2_svg_start, wifi2_svg_end);
DEFINE_URI_HANDLER("/wifi3.svg", "image/svg+xml", wifi3_svg_start, wifi3_svg_end);

static httpd_uri_t uri_handlers[] = {
    URI_HANDLER_ENTRY("/", index_html_start),
    URI_HANDLER_ENTRY("/index.html", index_html_start),
    URI_HANDLER_ENTRY("/style.css", style_css_start),
    URI_HANDLER_ENTRY("/code.js", code_js_start),
    URI_HANDLER_ENTRY("/lock.svg", lock_svg_start),
    URI_HANDLER_ENTRY("/wifi0.svg", wifi0_svg_start),
    URI_HANDLER_ENTRY("/wifi1.svg", wifi1_svg_start),
    URI_HANDLER_ENTRY("/wifi2.svg", wifi2_svg_start),
    URI_HANDLER_ENTRY("/wifi3.svg", wifi3_svg_start)
};

void http_app_start(void) {
    if (server) {
        ESP_LOGW(TAG, "HTTP server already running");
        return;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.max_uri_handlers = 16;
    config.stack_size = 8192;

    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return;
    }

    for (int i = 0; i < sizeof(uri_handlers)/sizeof(uri_handlers[0]); ++i) {
        httpd_register_uri_handler(server, &uri_handlers[i]);
    }

    ESP_LOGI(TAG, "HTTP captive-portal ready");
}
