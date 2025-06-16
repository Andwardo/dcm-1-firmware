/*
 * File: main/main.c
 *
 * Version: v8.0.13
 * Created on:
 * Last edited: 16 Jun 2025 18:47
 *
 * Author: R. Andrew Ballard (c)2025
 *
 */

/* C Standard Library */
#include <inttypes.h>

/* FreeRTOS (MUST be first) */
#include "freertos/FreeRTOS.h" // CORRECTED from .hh to .h
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
    ESP_LOGI(TAG, ">> DCM-1 v8.0.13 booting...");

    // Initialize all necessary services and components
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    board_manager_init();
    ESP_ERROR_CHECK(wifi_manager_init());
    app_logic_init(); // This starts the main application task

    // Start the HTTP server
    start_http_server();

    ESP_LOGI(TAG, "Initialization complete. Main task is finished.");

    // A task function must not return. The main task's job is done, so we delete it.
    vTaskDelete(NULL);
}
