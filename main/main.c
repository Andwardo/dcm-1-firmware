/*
 * File: main/main.c
 *
 * Version: v8.0.10
 * Author: R. Andrew Ballard (c)2025
 */

/* C Standard Library */
#include <inttypes.h>

/* FreeRTOS (MUST be first) */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ESP-IDF */
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_http_server.h"

/* Project Components */
#include "app_logic.h"
#include "wifi_manager.h"
#include "board_manager.h"
#include "mqtt_manager.h"

static const char *TAG = "DCM1";

static esp_err_t ping_handler(httpd_req_t *req) {
    const char resp[] = "pong";
    httpd_resp_send(req, resp, sizeof(resp) - 1);
    return ESP_OK;
}

static httpd_handle_t start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "[HTTP] Starting server...");
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "[HTTP] Failed to start server");
        return NULL;
    }
    ESP_LOGI(TAG, "[HTTP] Server started");
    httpd_uri_t ping_uri = {.uri = "/ping", .method = HTTP_GET, .handler  = ping_handler, .user_ctx = NULL};
    httpd_register_uri_handler(server, &ping_uri);
    ESP_LOGI(TAG, "[HTTP] Registered /ping handler");
    return server;
}

void app_main(void) {
    ESP_LOGI(TAG, ">> DCM-1 v8.0.10 booting...");

    // 1. Initialize NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // 2. Initialize TCP/IP stack & event loop
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // 3. Initialize board-specific peripherals
    board_manager_init();

    /*
     * CORRECTED ORDER: Initialize managers BEFORE starting application tasks
     * that depend on them.
     */
    // 4. Initialize Wi-Fi Manager (creates event group)
    ESP_ERROR_CHECK(wifi_manager_init());

    // 5. Initialize Application Logic (creates task that USES event group)
    app_logic_init();

    // The rest of this logic will be refactored in Phase 3
    if (!wifi_manager_has_saved_credentials()) {
        ESP_LOGW(TAG, "No saved credentials -- provisioning mode");
        wifi_manager_start_provisioning();
    } else {
        ESP_LOGI(TAG, "Found credentials -- connecting to AP");
        wifi_manager_connect();
    }

    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");
    while (!wifi_manager_is_ready()) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    ESP_LOGI(TAG, "Wi-Fi ready (mode=%s)",
             wifi_manager_is_sta_connected() ? "STA" : "AP");

    start_http_server();

    // Heartbeat loop is no longer needed here, as app_logic handles the main loop
    ESP_LOGI(TAG, "Initialization complete. Handing off to app_task.");
}
