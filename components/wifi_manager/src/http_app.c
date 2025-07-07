/*
 *  http_app.c
 *
 *  Created on: 2025-06-12
 *  Edited on: 2025-07-07 (local time)
 *      Author: Andwardo
 *      Version: v8.2.35
 */
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_spiffs.h"
#include "esp_vfs.h"
#include "esp_http_server.h"
#include "http_app.h"

#define TAG "http_app"

static httpd_handle_t server = NULL;

static esp_err_t serve_static_file(httpd_req_t *req, const char *filepath, const char *content_type) {
    FILE *file = fopen(filepath, "r");
    if (!file) {
        ESP_LOGE(TAG, "Failed to open file: %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read file");
        return ESP_FAIL;
    }

    fseek(file, 0, SEEK_END);
    size_t filesize = ftell(file);
    rewind(file);

    char *buffer = malloc(filesize + 1);
    if (!buffer) {
        fclose(file);
        ESP_LOGE(TAG, "Memory allocation failed");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory error");
        return ESP_FAIL;
    }

    fread(buffer, 1, filesize, file);
    fclose(file);

    buffer[filesize] = '\0';
    httpd_resp_set_type(req, content_type);
    httpd_resp_send(req, buffer, filesize);

    free(buffer);
    return ESP_OK;
}

static esp_err_t index_html_handler(httpd_req_t *req) {
    return serve_static_file(req, "/spiffs/index.html", "text/html");
}

static esp_err_t code_js_handler(httpd_req_t *req) {
    return serve_static_file(req, "/spiffs/code.js", "application/javascript");
}

esp_err_t http_app_start(void) {
    if (server != NULL) {
        ESP_LOGW(TAG, "HTTP server already started");
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t ret = httpd_start(&server, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ret;
    }

    httpd_uri_t index_html_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_html_handler,
        .user_ctx = NULL
    };

    httpd_uri_t code_js_uri = {
        .uri = "/code.js",
        .method = HTTP_GET,
        .handler = code_js_handler,
        .user_ctx = NULL
    };

    httpd_register_uri_handler(server, &index_html_uri);
    httpd_register_uri_handler(server, &code_js_uri);

    ESP_LOGI(TAG, "HTTP server started");
    return ESP_OK;
}