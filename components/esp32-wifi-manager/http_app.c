/*
 * File: components/esp32-wifi-manager/http_app.c
 *
 * This file contains the web server logic for the esp32-wifi-manager.
 *
 * Version: 7.1.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include <string.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mac.h" // <-- ADDED THIS LINE
#include "wifi_manager.h"

/* embedded binary data */
extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[]   asm("_binary_style_css_end");
extern const uint8_t code_js_start[] asm("_binary_code_js_start");
extern const uint8_t code_js_end[]   asm("_binary_code_js_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

static const char TAG[] = "http_app";

/* http server instance */
static httpd_handle_t httpd_handle = NULL;


static esp_err_t http_server_style_css_handler(httpd_req_t *req){
	ESP_LOGI(TAG, "style.css requested");
	httpd_resp_set_type(req, "text/css");
	httpd_resp_send(req, (const char*)style_css_start, style_css_end - style_css_start);
	return ESP_OK;
}

static esp_err_t http_server_code_js_handler(httpd_req_t *req){
	ESP_LOGI(TAG, "code.js requested");
	httpd_resp_set_type(req, "text/javascript");
	httpd_resp_send(req, (const char*)code_js_start, code_js_end - code_js_start);
	return ESP_OK;
}

static esp_err_t http_server_index_html_handler(httpd_req_t *req){
	ESP_LOGI(TAG, "index.html requested");
	httpd_resp_set_type(req, "text/html");
	httpd_resp_send(req, (const char*)index_html_start, index_html_end - index_html_start);
	return ESP_OK;
}


void http_app_start(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting HTTP Server");
    ESP_ERROR_CHECK(httpd_start(&httpd_handle, &config));

    // URI handlers
    httpd_uri_t style_css = { .uri = "/style.css", .method = HTTP_GET, .handler = http_server_style_css_handler, .user_ctx  = NULL };
    httpd_register_uri_handler(httpd_handle, &style_css);

    httpd_uri_t code_js = { .uri = "/code.js", .method = HTTP_GET, .handler = http_server_code_js_handler, .user_ctx  = NULL };
    httpd_register_uri_handler(httpd_handle, &code_js);

    httpd_uri_t index_html = { .uri = "/", .method = HTTP_GET, .handler = http_server_index_html_handler, .user_ctx  = NULL };
    httpd_register_uri_handler(httpd_handle, &index_html);
}

