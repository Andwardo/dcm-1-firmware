/*
 * File: components/esp32-wifi-manager/http_app.c
 * Implements the web server and API for Wi-Fi provisioning.
 *
 * Created on: 2025-06-17
 * Edited on:  2025-06-18
 *
 * Version: v8.2.9
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#include "http_app.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_http_server.h"
#include "nvs_flash.h"
#include "cJSON.h"
#include "esp_netif.h"
#include "lwip/dns.h"

static const char *TAG = "HTTP_APP";
static httpd_handle_t s_server = NULL;

extern const char index_html_start[] asm("_binary_index_html_start");
extern const char index_html_end[]   asm("_binary_index_html_end");
extern const char style_css_start[]  asm("_binary_style_css_start");
extern const char style_css_end[]    asm("_binary_style_css_end");
extern const char code_js_start[]    asm("_binary_code_js_start");
extern const char code_js_end[]      asm("_binary_code_js_end");

static esp_err_t root_get_handler(httpd_req_t *req);
static esp_err_t api_scan_get_handler(httpd_req_t *req);
static esp_err_t api_connect_post_handler(httpd_req_t *req);
static void start_dns_server(void);

esp_err_t http_app_start_provisioning_server(void) {
    if (s_server) return ESP_OK;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;
    config.task_priority = 4;

    ESP_LOGI(TAG, "Starting web server for provisioning...");
    if (httpd_start(&s_server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start web server");
        return ESP_FAIL;
    }

    // --- Register URI Handlers (most specific routes first) ---
    httpd_uri_t get_scan    = { .uri = "/api/wifi-scan", .method = HTTP_GET,  .handler = api_scan_get_handler };
    httpd_register_uri_handler(s_server, &get_scan);

    httpd_uri_t post_connect = { .uri = "/api/connect",   .method = HTTP_POST, .handler = api_connect_post_handler };
    httpd_register_uri_handler(s_server, &post_connect);
    
    // The catch-all handler for serving files MUST be last
    httpd_uri_t get_root    = { .uri = "/*",             .method = HTTP_GET,  .handler = root_get_handler };
    httpd_register_uri_handler(s_server, &get_root);

    start_dns_server();
    return ESP_OK;
}

// ... (The rest of the file is identical to the last version) ...
static void start_dns_server(void) {
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);
    ip_addr_t dns_ip;
    dns_ip.u_addr.ip4.addr = ip_info.ip.addr;
    dns_ip.type = IPADDR_TYPE_V4;
    dns_setserver(0, &dns_ip);
    ESP_LOGI(TAG, "DNS Server for captive portal started on " IPSTR, IP2STR(&ip_info.ip));
}

static esp_err_t root_get_handler(httpd_req_t *req) {
    if (strcmp(req->uri, "/style.css") == 0) {
        httpd_resp_set_type(req, "text/css");
        return httpd_resp_send(req, style_css_start, style_css_end - style_css_start);
    } else if (strcmp(req->uri, "/code.js") == 0) {
        httpd_resp_set_type(req, "application/javascript");
        return httpd_resp_send(req, code_js_start, code_js_end - code_js_start);
    }
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, index_html_start, index_html_end - index_html_start);
}

static esp_err_t api_scan_get_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "API: Scanning for Wi-Fi networks...");
    uint16_t num_aps = 0;
    wifi_scan_config_t scan_config = { .show_hidden = false, .scan_type = WIFI_SCAN_TYPE_ACTIVE };
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config, true));
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&num_aps));
    if (num_aps == 0) {
        httpd_resp_set_type(req, "application/json");
        return httpd_resp_send(req, "[]", 2);
    }
    wifi_ap_record_t *ap_list = malloc(sizeof(wifi_ap_record_t) * num_aps);
    if(ap_list == NULL) return ESP_ERR_NO_MEM;
    ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&num_aps, ap_list));

    cJSON *root = cJSON_CreateArray();
    for (int i = 0; i < num_aps; i++) {
        if (strlen((char*)ap_list[i].ssid) > 0) {
            cJSON *net = cJSON_CreateObject();
            cJSON_AddStringToObject(net, "ssid", (char *)ap_list[i].ssid);
            cJSON_AddNumberToObject(net, "rssi", ap_list[i].rssi);
            cJSON_AddBoolToObject(net, "auth", ap_list[i].authmode != WIFI_AUTH_OPEN);
            cJSON_AddItemToArray(root, net);
        }
    }
    free(ap_list);

    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root);
    
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_str, strlen(json_str));

    free(json_str);
    ESP_LOGI(TAG, "API: Scan complete.");
    return ESP_OK;
}

static esp_err_t api_connect_post_handler(httpd_req_t *req) {
    char content[256];
    int recv_len = httpd_req_recv(req, content, sizeof(content) - 1);
    if (recv_len <= 0) { return ESP_FAIL; }
    content[recv_len] = '\0';
    cJSON *json = cJSON_Parse(content);
    if (json == NULL) { httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON"); return ESP_FAIL; }
    const cJSON *ssid_json = cJSON_GetObjectItem(json, "ssid");
    const cJSON *pass_json = cJSON_GetObjectItem(json, "password");
    if (!cJSON_IsString(ssid_json) || !cJSON_IsString(pass_json)) {
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing SSID or Password");
        return ESP_FAIL;
    }
    ESP_LOGI(TAG, "API: Received credentials for SSID: %s", ssid_json->valuestring);
    
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(nvs_open("wifi_cred", NVS_READWRITE, &nvs_handle));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", ssid_json->valuestring));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "password", pass_json->valuestring));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
    cJSON_Delete(json);
    
    const char *resp = "{\"success\":true}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, resp);
    
    ESP_LOGI(TAG, "API: Credentials saved. Rebooting device in 2 seconds...");
    vTaskDelay(pdMS_TO_TICKS(2000));
    esp_restart();
    return ESP_OK;
}
