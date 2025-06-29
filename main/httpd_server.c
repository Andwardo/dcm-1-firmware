/*
 * Project:    PianoGuard_DCM-1
 * File:       main/httpd_server.c
 * Version:    8.4.2
 * Author:     R. Andrew Ballard
 * Date:       Jun 30 2025
 *
 * HTTP captive-portal server serving files from SPIFFS.
 */

#include "httpd_server.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "esp_spiffs.h"
#include "cJSON.h"
#include "esp_wifi.h"
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static const char *TAG = "HTTPD";
static httpd_handle_t server = NULL;

/* URI handler prototypes */
static esp_err_t serve_file(httpd_req_t *req, const char *path, const char *content_type);
static esp_err_t root_get_handler     (httpd_req_t *req);
static esp_err_t style_get_handler    (httpd_req_t *req);
static esp_err_t code_get_handler     (httpd_req_t *req);
static esp_err_t scan_post_handler    (httpd_req_t *req);
static esp_err_t connect_post_handler (httpd_req_t *req);
static esp_err_t favicon_get_handler  (httpd_req_t *req);

/**
 * @brief Mount SPIFFS partition at /spiffs.
 */
static esp_err_t mount_spiffs(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = "spiffs",
        .max_files = 5,
        .format_if_mount_failed = false
    };
    esp_err_t err = esp_vfs_spiffs_register(&conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(err));
    } else {
        size_t total = 0, used = 0;
        esp_spiffs_info(conf.partition_label, &total, &used);
        ESP_LOGI(TAG, "SPIFFS mounted: total=%"PRIu32", used=%"PRIu32, total, used);
    }
    return err;
}

esp_err_t start_http_server(void)
{
    ESP_LOGI(TAG, ">> start_http_server()");
    if (mount_spiffs() != ESP_OK) {
        return ESP_FAIL;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    ESP_LOGI(TAG, "   port=%d, stack=%d", config.server_port, config.stack_size);

    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return err;
    }

    const httpd_uri_t uris[] = {
        { "/",            HTTP_GET,  root_get_handler,     NULL },
        { "/style.css",   HTTP_GET,  style_get_handler,    NULL },
        { "/code.js",     HTTP_GET,  code_get_handler,     NULL },
        { "/scan",        HTTP_POST, scan_post_handler,    NULL },
        { "/connect",     HTTP_POST, connect_post_handler, NULL },
        { "/favicon.ico", HTTP_GET,  favicon_get_handler,  NULL },
    };
    for (size_t i = 0; i < sizeof(uris)/sizeof(uris[0]); ++i) {
        httpd_register_uri_handler(server, &uris[i]);
        ESP_LOGI(TAG, "   registered %s", uris[i].uri);
    }
    ESP_LOGI(TAG, "<< start_http_server() done");
    return ESP_OK;
}

esp_err_t stop_http_server(void)
{
    esp_vfs_spiffs_unregister(NULL);
    return httpd_stop(server);
}

/**
 * @brief Generic file‐serving helper.
 */
static esp_err_t serve_file(httpd_req_t *req, const char *path, const char *content_type)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        ESP_LOGE(TAG, "Failed to open '%s'", path);
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "File not found");
        return ESP_FAIL;
    }

    struct stat st;
    fstat(fd, &st);
    httpd_resp_set_type(req, content_type);
    const size_t buf_size = 1024;
    char* buf = malloc(buf_size);
    ssize_t r;
    while ((r = read(fd, buf, buf_size)) > 0) {
        httpd_resp_send_chunk(req, buf, r);
    }
    free(buf);
    close(fd);
    httpd_resp_send_chunk(req, NULL, 0);  // end response
    return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    return serve_file(req, "/spiffs/index.html", "text/html");
}

static esp_err_t style_get_handler(httpd_req_t *req)
{
    return serve_file(req, "/spiffs/style.css", "text/css");
}

static esp_err_t code_get_handler(httpd_req_t *req)
{
    return serve_file(req, "/spiffs/code.js", "application/javascript");
}

static esp_err_t scan_post_handler(httpd_req_t *req)
{
    wifi_scan_config_t cfg = { .show_hidden = true };
    esp_wifi_scan_start(&cfg, true);

    uint16_t n = 0;
    esp_wifi_scan_get_ap_num(&n);
    wifi_ap_record_t *recs = malloc(sizeof(*recs) * n);
    esp_wifi_scan_get_ap_records(&n, recs);

    cJSON *arr = cJSON_CreateArray();
    for (int i = 0; i < n; i++) {
        cJSON *o = cJSON_CreateObject();
        cJSON_AddStringToObject(o, "ssid", (char*)recs[i].ssid);
        cJSON_AddNumberToObject(o, "rssi", recs[i].rssi);
        cJSON_AddItemToArray(arr, o);
    }
    char *out = cJSON_Print(arr);
    cJSON_Delete(arr);
    free(recs);

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, out, strlen(out));
    free(out);
    return ESP_OK;
}

static esp_err_t connect_post_handler(httpd_req_t *req)
{
    char buf[128];
    int to_read = MIN(req->content_len, sizeof(buf) - 1);
    int r = httpd_req_recv(req, buf, to_read);
    if (r <= 0) {
        return HTTPD_SOCK_ERR_TIMEOUT;
    }
    buf[r] = '\0';

    cJSON *j = cJSON_Parse(buf);
    const char *ssid = cJSON_GetObjectItem(j, "ssid")->valuestring;
    const char *pwd  = cJSON_GetObjectItem(j, "password")->valuestring;
    cJSON_Delete(j);

    ESP_LOGI(TAG, "Connecting to \"%s\"", ssid);
    wifi_config_t wc = {0};
    strncpy((char*)wc.sta.ssid, ssid, sizeof(wc.sta.ssid));
    strncpy((char*)wc.sta.password, pwd, sizeof(wc.sta.password));
    esp_wifi_set_config(WIFI_IF_STA, &wc);
    esp_wifi_connect();

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"success\":true}");
    esp_restart();
    return ESP_OK;  // never reached
}

static esp_err_t favicon_get_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "204 No Content");
    return httpd_resp_send(req, NULL, 0);
}

