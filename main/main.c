/**
 * File: main.c
 * Description: Main entry point for the PianoGuard DCM-1 application.
 * Created on: 2025-06-25
 * Edited on:  2025-06-30
 * Version: v8.7.0
 * Author: R. Andrew Ballard (c) 2025
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Your Custom Component Headers
#include "mqtt_manager.h"
#include "cert_loader.h"
#include "board_manager.h"
#include "wifi_manager.h"
#include "app_logic.h"

static const char *TAG = "PIANOGUARD_MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Starting PianoGuard Firmware v8.7.0");

    // 1. Initialize NVS (must be first)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize Hardware Abstraction Layer
    ESP_ERROR_CHECK(board_manager_init());

    // 3. Start the Wi-Fi Manager. It will handle provisioning and connecting.
    wifi_manager_init();

    // 4. Start the main application logic task. It will wait internally for
    //    the Wi-Fi Manager to establish a connection before running its main loop.
    app_logic_init();

    ESP_LOGI(TAG, "app_main initialization complete. Handing off control to component tasks.");
}
