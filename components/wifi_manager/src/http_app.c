/*
 *  http_app.c
 *
 *  Created on: 2025-06-12
 *  Edited on: 2025-07-07
 *      Author: Andwardo
 *      Version: v8.2.36
 */

#include "http_app.h"
#include <esp_log.h>
#include <esp_err.h>
#include <string.h>
#include <sys/stat.h>
#include "esp_vfs.h"
#include "esp_spiffs.h"

static const char *TAG = "http_app";

esp_err_t start_http_server() {
    ESP_LOGI(TAG, "Mounting SPIFFS...");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = true
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount SPIFFS (%s)", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "SPIFFS mounted. Static files should be available.");

    return ESP_OK;
}

void http_app_start(void) {
    ESP_LOGI(TAG, "Starting HTTP app...");

    if (start_http_server() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return;
    }

    ESP_LOGI(TAG, "HTTP server started (static serving only)");
}