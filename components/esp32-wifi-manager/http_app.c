/*
 * File: components/esp32-wifi-manager/http_app.c
 *
 * Created on: 11 June 2025 08:35:00
 * Last edited on: 12 June 2025 09:50:00
 *
 * Version: 7.5.2
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include <string.h>
#include <stdlib.h>
#include "sys/param.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mac.h"
#include "wifi_manager.h"
#include "cJSON.h"
#include "http_app.h" // Include the new private header

/* embedded binary data */
extern const uint8_t style_css_start[] asm("_binary_style_css_start");
extern const uint8_t style_css_end[]   asm("_binary_style_css_end");
extern const uint8_t code_js_start[] asm("_binary_code_js_start");
extern const uint8_t code_js_end[]   asm("_binary_code_js_end");
extern const uint8_t index_html_start[] asm("_binary_index_html_start");
extern const uint8_t index_html_end[]   asm("_binary_index_html_end");

static const char TAG[] = "http_app";
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

static esp_err_t http_server_wifi_scan_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "/api/wifi-scan requested");
    
    wifi_scan_config_t scan_config = {
        .ssid = 0, .bssid = 0, .channel = 0, .show_hidden = false
    };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));

    uint16_t ap_count = 0;
    esp_wifi_scan_get_ap_num(&ap_count);
    if (ap_count == 0) {
        ESP_LOGI(TAG, "No networks found");
        httpd_resp_send(req, "[]", HTTPD_RESP_USE_STRLEN);
        return ESP_OK;
    }

    wifi_ap_record_t *ap_info = (wifi_ap_record_t *)malloc(ap_count * sizeof(wifi_ap_record_t));
    if (ap_info == NULL) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_count, ap_info));

    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < ap_count; i++) {
        cJSON *net = cJSON_CreateObject();
        cJSON_AddStringToObject(net, "ssid", (char *)ap_info[i].ssid);
        cJSON_AddNumberToObject(net, "rssi", ap_info[i].rssi);
        cJSON_AddBoolToObject(net, "auth", ap_info[i].authmode != WIFI_AUTH_OPEN);
        cJSON_AddItemToArray(root, net);
    }

    char *json_string = cJSON_PrintUnformatted(root);
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_string, HTTPD_RESP_USE_STRLEN);

    free(json_string);
    free(ap_info);
    cJSON_Delete(root);

    return ESP_OK;
}

static esp_err_t http_server_connect_handler(httpd_req_t *req) {
    char buf[128];
    int ret, remaining = req->content_len;

    if (remaining > sizeof(buf)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Request too long");
        return ESP_FAIL;
    }

    while (remaining > 0) {
        if ((ret = httpd_req_recv(req, buf, MIN(remaining, sizeof(buf)))) <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
    }
    buf[req->content_len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (root == NULL) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    const cJSON *ssid_json = cJSON_GetObjectItem(root, "ssid");
    const cJSON *password_json = cJSON_GetObjectItem(root, "password");

    if (!cJSON_IsString(ssid_json) || !cJSON_IsString(password_json)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing ssid or password");
        return ESP_FAIL;
    }
    
    wifi_manager_save_credentials_and_restart(ssid_json->valuestring, password_json->valuestring);

    cJSON_Delete(root);

    const char *resp_str = "{\"success\": true}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t http_server_any_handler(httpd_req_t *req) {
    httpd_resp_set_status(req, "302 Found");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_send(req, NULL, 0);
    return ESP_OK;
}

void http_app_start(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting HTTP Server");
    ESP_ERROR_CHECK(httpd_start(&httpd_handle, &config));

    httpd_uri_t style_css = { .uri = "/style.css", .method = HTTP_GET, .handler = http_server_style_css_handler };
    httpd_register_uri_handler(httpd_handle, &style_css);

    httpd_uri_t code_js = { .uri = "/code.js", .method = HTTP_GET, .handler = http_server_code_js_handler };
    httpd_register_uri_handler(httpd_handle, &code_js);

    httpd_uri_t index_html = { .uri = "/", .method = HTTP_GET, .handler = http_server_index_html_handler };
    httpd_register_uri_handler(httpd_handle, &index_html);
    
    httpd_uri_t wifi_scan = { .uri = "/api/wifi-scan", .method = HTTP_GET, .handler = http_server_wifi_scan_handler };
    httpd_register_uri_handler(httpd_handle, &wifi_scan);
    
    httpd_uri_t connect = { .uri = "/api/connect", .method = HTTP_POST, .handler = http_server_connect_handler };
    httpd_register_uri_handler(httpd_handle, &connect);

    httpd_uri_t any_other = { .uri = "/*", .method = HTTP_GET, .handler = http_server_any_handler };
    httpd_register_uri_handler(httpd_handle, &any_other);
}
