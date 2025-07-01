/**
 * File: http_app.c
 * Description: Manages the captive portal web server and DNS for Wi-Fi provisioning.
 * Created on: 2025-06-25
 * Edited on:  2025-06-30
 * Version: v8.5.8
 * Author: R. Andrew Ballard (c) 2025
 */

#include "http_app.h"
#include <string.h>
#include <fcntl.h>
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "wifi_manager.h"
#include "dns_server.h"

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

static const char *TAG = "HTTP_APP";

// --- Forward Declarations ---
static esp_err_t serve_spiffs_file(httpd_req_t *req, const char* filepath, const char* content_type);
static esp_err_t root_handler(httpd_req_t *req);
static esp_err_t wifi_scan_handler(httpd_req_t *req);
static esp_err_t wifi_connect_handler(httpd_req_t *req);


/**
 * @brief Starts the HTTP server and registers URI handlers.
 */
void http_app_start(bool lru_purge_enable) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    ESP_LOGI(TAG, "Starting HTTP server...");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri = "/", .method = HTTP_GET, .handler = root_handler, .user_ctx = NULL });

    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri = "/api/wifi-scan.json", .method = HTTP_GET, .handler = wifi_scan_handler, .user_ctx = NULL });

    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri = "/api/connect.json", .method = HTTP_POST, .handler = wifi_connect_handler, .user_ctx = NULL });

    httpd_register_uri_handler(server, &(httpd_uri_t){
        .uri = "/*", .method = HTTP_GET, .handler = root_handler, .user_ctx = NULL });

    dns_server_start();
}


/**
 * @brief Serves a static file from SPIFFS to the client.
 */
static esp_err_t serve_spiffs_file(httpd_req_t *req, const char* filepath, const char* content_type) {
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/spiffs%s", filepath);

    FILE *f = fopen(full_path, "r");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open file: %s", full_path);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, content_type);

    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
            fclose(f);
            ESP_LOGE(TAG, "File sending failed!");
            return ESP_FAIL;
        }
    }

    httpd_resp_send_chunk(req, NULL, 0); // Final empty chunk
    fclose(f);
    return ESP_OK;
}


/**
 * @brief URI handler for root and static assets.
 */
static esp_err_t root_handler(httpd_req_t *req) {
    if (strcmp(req->uri, "/") == 0 || strcmp(req->uri, "/index.html") == 0)
        return serve_spiffs_file(req, "/index.html", "text/html");
    if (strcmp(req->uri, "/style.css") == 0)
        return serve_spiffs_file(req, "/style.css", "text/css");
    if (strcmp(req->uri, "/code.js") == 0)
        return serve_spiffs_file(req, "/code.js", "application/javascript");
    if (strstr(req->uri, ".svg") != NULL)
        return serve_spiffs_file(req, req->uri, "image/svg+xml");

    ESP_LOGW(TAG, "Not found: %s", req->uri);
    httpd_resp_send_404(req);
    return ESP_FAIL;
}


/**
 * @brief Placeholder for Wi-Fi scan API.
 */
static esp_err_t wifi_scan_handler(httpd_req_t *req) {
    httpd_resp_send(req, "[]", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}


/**
 * @brief Handles POST request with SSID/password and sends message to Wi-Fi manager.
 */
static esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char buf[256];
    int ret = httpd_req_recv(req, buf, MIN(req->content_len, sizeof(buf) - 1));
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (!root) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    cJSON *ssid_json = cJSON_GetObjectItem(root, "ssid");
    cJSON *password_json = cJSON_GetObjectItem(root, "password");
    if (!ssid_json || !password_json || !cJSON_IsString(ssid_json) || !cJSON_IsString(password_json)) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing SSID or password");
        return ESP_FAIL;
    }

    const char *ssid = ssid_json->valuestring;
    const char *password = password_json->valuestring;
    ESP_LOGI(TAG, "Received credentials for SSID: %s", ssid);

    wifi_manager_message_t msg;
    memset(&msg.sta_config, 0, sizeof(wifi_config_t));
    strlcpy((char *)msg.sta_config.sta.ssid, ssid, sizeof(msg.sta_config.sta.ssid));
    strlcpy((char *)msg.sta_config.sta.password, password, sizeof(msg.sta_config.sta.password));
    msg.msg_id = WIFI_MANAGER_MSG_CONNECT_STA;

    if (wifi_manager_send_message(&msg) != pdTRUE) {
        cJSON_Delete(root);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to queue connect");
        return ESP_FAIL;
    }

    cJSON_Delete(root);
    httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
