/**
 * File: http_app.c
 * Description: Manages the captive portal web server and DNS for Wi-Fi provisioning.
 * Created on: 2025-06-25
 * Edited on:  2025-06-30
 * Version: v8.5.5
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
#include "wifi_manager.h" // For messaging the wifi manager
#include "dns_server.h"   // For the captive portal DNS server

static const char *TAG = "HTTP_APP";

// --- URI Handlers ---
static esp_err_t root_handler(httpd_req_t *req);
static esp_err_t wifi_scan_handler(httpd_req_t *req);
static esp_err_t wifi_connect_handler(httpd_req_t *req);

// --- Helper Functions ---
static esp_err_t serve_spiffs_file(httpd_req_t *req, const char* filepath, const char* content_type);

/**
 * @brief Starts the web server.
 */
void http_app_start(bool lru_purge_enable) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard; // Enable wildcard matching

    ESP_LOGI(TAG, "Starting HTTP server...");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    // URI handler for the root page
    httpd_uri_t root_uri = { .uri = "/", .method = HTTP_GET, .handler = root_handler, .user_ctx = NULL };
    httpd_register_uri_handler(server, &root_uri);

    // URI handler for Wi-Fi scan API
    httpd_uri_t scan_uri = { .uri = "/api/wifi-scan.json", .method = HTTP_GET, .handler = wifi_scan_handler, .user_ctx = NULL };
    httpd_register_uri_handler(server, &scan_uri);

    // URI handler for Wi-Fi connect API
    httpd_uri_t connect_uri = { .uri = "/api/connect.json", .method = HTTP_POST, .handler = wifi_connect_handler, .user_ctx = NULL };
    httpd_register_uri_handler(server, &connect_uri);

    // URI handler for all other static assets (CSS, JS, images)
    httpd_uri_t assets_uri = { .uri = "/*", .method = HTTP_GET, .handler = root_handler, .user_ctx = NULL };
    httpd_register_uri_handler(server, &assets_uri);

    // Start the DNS server for the captive portal
    dns_server_start();
}

/**
 * @brief Serves a file from the /spiffs partition.
 */
static esp_err_t serve_spiffs_file(httpd_req_t *req, const char* filepath, const char* content_type) {
    char full_path[128];
    snprintf(full_path, sizeof(full_path), "/spiffs%s", filepath);

    // Open the file
    FILE *f = fopen(full_path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file: %s", full_path);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    // Set the content type
    httpd_resp_set_type(req, content_type);

    // Read and send the file in chunks
    char buffer[1024];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (httpd_resp_send_chunk(req, buffer, bytes_read) != ESP_OK) {
            fclose(f);
            ESP_LOGE(TAG, "File sending failed!");
            return ESP_FAIL;
        }
    }

    // Finish the response
    httpd_resp_send_chunk(req, NULL, 0);
    fclose(f);
    return ESP_OK;
}

/**
 * @brief Handler for all GET requests. Serves static files.
 */
static esp_err_t root_handler(httpd_req_t *req) {
    if (strcmp(req->uri, "/") == 0 || strcmp(req->uri, "/index.html") == 0) {
        return serve_spiffs_file(req, "/index.html", "text/html");
    } else if (strcmp(req->uri, "/style.css") == 0) {
        return serve_spiffs_file(req, "/style.css", "text/css");
    } else if (strcmp(req->uri, "/code.js") == 0) {
        return serve_spiffs_file(req, "/code.js", "application/javascript");
    } else if (strstr(req->uri, ".svg") != NULL) {
        return serve_spiffs_file(req, req->uri, "image/svg+xml");
    }

    ESP_LOGW(TAG, "Not found: %s", req->uri);
    httpd_resp_send_404(req);
    return ESP_FAIL;
}

/**
 * @brief Handler for the Wi-Fi scan API call.
 */
static esp_err_t wifi_scan_handler(httpd_req_t *req) {
    // ... (implementation to scan for networks and return JSON) ...
    // This logic can be copied from your original http_app.c
    httpd_resp_send(req, "[]", HTTPD_RESP_USE_STRLEN); // Placeholder
    return ESP_OK;
}

/**
 * @brief Handler for the Wi-Fi connect API call.
 */
static esp_err_t wifi_connect_handler(httpd_req_t *req) {
    char buf[256];
    int ret, remaining = req->content_len;

    if (remaining >= sizeof(buf)) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Request content too long");
        return ESP_FAIL;
    }

    // Read the POST data
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    // Parse the JSON
    cJSON *root = cJSON_Parse(buf);
    const char *ssid = cJSON_GetObjectItem(root, "ssid")->valuestring;
    const char *password = cJSON_GetObjectItem(root, "password")->valuestring;

    if (ssid && password) {
        ESP_LOGI(TAG, "Received credentials for SSID: %s", ssid);
        wifi_manager_connect_async(ssid, password);
        httpd_resp_send(req, "{\"status\":\"ok\"}", HTTPD_RESP_USE_STRLEN);
    } else {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing SSID or password");
    }

    cJSON_Delete(root);
    return ESP_OK;
}
