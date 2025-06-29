/*
Copyright (c) 2017-2020 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

// Fallback defaults if not set in sdkconfig
#ifndef CONFIG_WEBAPP_LOCATION
#define CONFIG_WEBAPP_LOCATION "/spiffs"
#endif

#ifndef DEFAULT_AP_IP
#define DEFAULT_AP_IP "192.168.4.1"
#endif

#include <stdlib.h>
#include <stdint.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include "esp_netif.h"
#include <esp_http_server.h>

#include "wifi_manager.h"
#include "http_app.h"

/* @brief tag used for ESP serial console messages */
static const char* TAG = "http_app";

/* forward declarations */
static esp_err_t http_server_get_handler(httpd_req_t *req);
static esp_err_t http_server_post_handler(httpd_req_t *req);
static esp_err_t http_server_delete_handler(httpd_req_t *req);
static esp_err_t http_app_generate_url(httpd_req_t *req, char *buffer, size_t maxlen);
static void http_app_send_image(httpd_req_t *req, const char *file_path);
static void http_app_start(httpd_handle_t server);

/* start the HTTP server */
void http_app_start_server(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG, "Starting HTTP server");
    if (httpd_start(&server, &config) == ESP_OK) {
        http_app_start(server);
    }
}

static void http_app_start(httpd_handle_t server) {
    httpd_uri_t uris[] = {
        {
            .uri       = "/api",
            .method    = HTTP_GET,
            .handler   = http_server_get_handler,
            .user_ctx  = NULL
        },
        {
            .uri       = "/api",
            .method    = HTTP_POST,
            .handler   = http_server_post_handler,
            .user_ctx  = NULL
        },
        {
            .uri       = "/api",
            .method    = HTTP_DELETE,
            .handler   = http_server_delete_handler,
            .user_ctx  = NULL
        }
    };

    for (int i = 0; i < sizeof(uris)/sizeof(httpd_uri_t); i++) {
        httpd_register_uri_handler(server, &uris[i]);
    }

    // Serve webapp files
    httpd_uri_t webapp = {
        .uri       = "/*",
        .method    = HTTP_GET,
        .handler   = httpd_resp_send_file,
        .user_ctx  = WEBAPP_LOCATION
    };
    httpd_register_uri_handler(server, &webapp);
}

static esp_err_t http_server_get_handler(httpd_req_t *req) {
    const char *host = httpd_req_get_hdr_value(req, "Host");
    wifi_manager_lock_sta_ip_string(portMAX_DELAY);
    bool access_from_sta_ip = host != NULL 
        ? (strstr(host, wifi_manager_get_sta_ip_string()) != NULL)
        : false;
    wifi_manager_unlock_sta_ip_string();

    if (host != NULL && !strstr(host, DEFAULT_AP_IP) && !access_from_sta_ip) {
        // not on AP or STA, redirect
        char redirect_url[64];
        snprintf(redirect_url, sizeof(redirect_url),
                 "http://%s:%d/", DEFAULT_AP_IP, HTTPD_SERVER_PORT);
        httpd_resp_set_status(req, "307 Temporary Redirect");
        httpd_resp_set_hdr(req, "Location", redirect_url);
        return ESP_OK;
    }

    // Normal GET handling
    char buf[128];
    http_app_generate_url(req, buf, sizeof(buf));
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, buf, strlen(buf));
}

static esp_err_t http_server_post_handler(httpd_req_t *req) {
    char buf[256];
    size_t len = httpd_req_recv(req, buf, sizeof(buf));
    if (len <= 0) {
        return ESP_FAIL;
    }
    buf[len] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (!root) {
        return ESP_FAIL;
    }

    cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
    cJSON *password = cJSON_GetObjectItem(root, "password");
    size_t ssid_len = ssid ? strlen(ssid->valuestring) : 0;
    size_t password_len = password ? strlen(password->valuestring) : 0;

    if (ssid_len && ssid_len <= MAX_SSID_LEN
        && password_len && password_len <= MAX_PASSWORD_LEN) {
        wifi_config_t *config = wifi_manager_get_wifi_sta_config();
        strncpy((char *)config->sta.ssid, ssid->valuestring, ssid_len+1);
        strncpy((char *)config->sta.password, password->valuestring, password_len+1);

        wifi_manager_disconnect_async();
        wifi_manager_connect_async();
        cJSON_Delete(root);
        httpd_resp_set_status(req, "204 No Content");
        return ESP_OK;
    }

    cJSON_Delete(root);
    httpd_resp_set_status(req, "400 Bad Request");
    return ESP_FAIL;
}

static esp_err_t http_server_delete_handler(httpd_req_t *req) {
    wifi_manager_disconnect_async();
    httpd_resp_set_status(req, "204 No Content");
    return ESP_OK;
}

static esp_err_t http_app_generate_url(httpd_req_t *req, char *buffer, size_t maxlen) {
    ip4_addr_t ip;
    wifi_manager_lock_json_buffer(portMAX_DELAY);
    char *json = wifi_manager_get_ip_info_json();
    wifi_manager_unlock_json_buffer();

    snprintf(buffer, maxlen, "%s", json);
    return ESP_OK;
}

static void http_app_send_image(httpd_req_t *req, const char *file_path) {
    extern const uint8_t cert_start[] asm("_binary_" CONFIG_WEBAPP_LOCATION "/" file_path "_start");
    extern const uint8_t cert_end[]   asm("_binary_" CONFIG_WEBAPP_LOCATION "/" file_path "_end");
    size_t size = cert_end - cert_start;
    httpd_resp_set_type(req, "image/png");
    httpd_resp_send(req, (const char *)cert_start, size);
}
