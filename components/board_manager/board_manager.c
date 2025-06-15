/*
 * File: components/board_manager/board_manager.c
 *
 * Created on: 12 June 2025 23:30:00
 * Last Edited on: 15 June 2025 14:10:00 CDT
 *
 * Version: 8.1.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "board_manager.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "BOARD_MANAGER";

/**
 * @brief Initialize GPIO peripherals (LED + Reset Button).
 */
esp_err_t board_manager_init(void)
{
    gpio_config_t output_conf = {
        .pin_bit_mask = (1ULL << STATUS_LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&output_conf);

    gpio_config_t button_conf = {
        .pin_bit_mask = (1ULL << WIFI_RESET_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&button_conf);

    ESP_LOGI(TAG, "Board peripherals initialized.");
    return ESP_OK;
}

/**
 * @brief Read the GPIO state of the Wi-Fi reset button.
 * @return true if button is pressed (active low), false otherwise.
 */
bool board_manager_is_reset_button_pressed(void)
{
    return gpio_get_level(WIFI_RESET_PIN) == 0;
}

/**
 * @brief If reset button is held during boot for >3 seconds,
 *        erase Wi-Fi credentials and reboot into provisioning mode.
 *
 *        This function should be called **before** wifi_manager_start().
 */
void board_manager_check_reset_button_hold(void)
{
    const TickType_t poll_interval = pdMS_TO_TICKS(100);   // Check every 100ms
    const TickType_t hold_time = pdMS_TO_TICKS(3000);      // Require 3s hold

    TickType_t start_time = xTaskGetTickCount();

    ESP_LOGI(TAG, "Checking for long reset button press...");

    // Wait while button is pressed, timing the duration
    while (gpio_get_level(WIFI_RESET_PIN) == 0) {
        if ((xTaskGetTickCount() - start_time) >= hold_time) {

            ESP_LOGW(TAG, "Reset button held >3s — clearing saved credentials.");

            // Erase Wi-Fi credentials stored in NVS
            nvs_handle_t nvs;
            if (nvs_open("wifi_creds", NVS_READWRITE, &nvs) == ESP_OK) {
                nvs_erase_all(nvs);
                nvs_commit(nvs);
                nvs_close(nvs);
                ESP_LOGI(TAG, "Credentials erased successfully.");
            } else {
                ESP_LOGE(TAG, "Failed to open NVS for credential erase.");
            }

            // Brief delay to allow logs to flush
            vTaskDelay(pdMS_TO_TICKS(500));

            ESP_LOGI(TAG, "Rebooting into AP mode...");
            esp_restart();
        }

        // Delay between button polls
        vTaskDelay(poll_interval);
    }

    // Button was released before threshold
    ESP_LOGI(TAG, "No long press detected. Continuing normal boot.");
}
