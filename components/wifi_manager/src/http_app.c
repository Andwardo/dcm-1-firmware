/*
 *  http_app.c
 *
 *  Created on: 2025-06-12
 *  Edited on: 2025-07-06 (local time)
 *      Author: Andwardo
 *      Version: v8.2.33
 */

#include <string.h>
#include <sys/stat.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_vfs.h"

#define TAG "http_app"

void http_app_start(void) {
    ESP_LOGI(TAG, "http_app_start() stub called.");
}

static esp_err_t serve_static_file(httpd_req_t *req, const char *filepath, const char *content_type) {
    FILE *f = fopen(filepath, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to open file");
        return ESP_FAIL;
    }

    struct stat st;
    if (stat(filepath, &st) != 0) {
        fclose(f);
        ESP_LOGE(TAG, "Failed to stat file: %s", filepath);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "File stat failed");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, content_type);
    char *buf = malloc(st.st_size);
    if (!buf) {
        fclose(f);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Memory error");
        return ESP_FAIL;
    }

    fread(buf, 1, st.st_size, f);
    fclose(f);
    esp_err_t err = httpd_resp_send(req, buf, st.st_size);
    free(buf);
    return err;
}

static esp_err_t code_js_handler(httpd_req_t *req) {
    return serve_static_file(req, "/spiffs/code.js", "application/javascript");
}

static esp_err_t index_html_handler(httpd_req_t *req) {
    return serve_static_file(req, "/spiffs/index.html", "text/html");
}

esp_err_t start_http_server(httpd_handle_t *server) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t ret = httpd_start(server, &config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return ret;
    }

    httpd_uri_t code_js_uri = {
        .uri      = "/code.js",
        .method   = HTTP_GET,
        .handler  = code_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(*server, &code_js_uri);

    httpd_uri_t index_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = index_html_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(*server, &index_uri);

    return ESP_OK;
}
