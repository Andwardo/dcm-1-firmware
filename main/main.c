/*
 * Project:   PianoGuard_DCM-1
 * File:      main/main.c
 * Version:   8.3.0
 * Author:    R. Andrew Ballard
 * Date:      June 23, 2025
 *
 * Main application entry point for PianoGuard.
 */
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_spiffs.h" // For esp_spiffs_info
#include "mqtt_manager.h"
#include "cert_loader.h"

// Forward declarations
void start_http_server(void);
// void start_mqtt_manager(void);

static const char *TAG = "PianoGuard_Main";

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize networking
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    // Assign pointers to the embedded certificates
    if (load_certificates_from_flash() != ESP_OK) {
        ESP_LOGE(TAG, "Failed to load certificates! Halting.");
        while(1) { vTaskDelay(pdMS_TO_TICKS(1000)); }
    }

    // Start application services that require certificates
    mqtt_manager_start();

    ESP_LOGI(TAG, "System setup complete.");
    while (true) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}
