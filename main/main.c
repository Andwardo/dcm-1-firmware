/*
 * File: main/main.c
 *
 * Created on: 10 June 2025 17:55:00
 * Last edited on: 10 June 2025 20:18:00
 *
 * Version: 7.1.2
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "nvs_flash.h"

// Include the header from our new library component
#include "wifi_manager.h"

// Logging Tag
static const char *TAG = "PIANOGUARD_MAIN";

// --- Pin Definitions for ESP32-S3 ---
#define I2C_SCL_PIN                 GPIO_NUM_6
#define I2C_SDA_PIN                 GPIO_NUM_5
// ... other pins ...
#define STATUS_LED_PIN              GPIO_NUM_2


void app_main(void)
{
    ESP_LOGI(TAG, "PianoGuard DCM-1 Starting Up...");

    // The wifi_manager library handles its own NVS initialization.
    // We can start it directly.
    wifi_manager_start();

    ESP_LOGI(TAG, "Web server for Wi-Fi provisioning is running.");
    ESP_LOGI(TAG, "Connect to Wi-Fi 'PianoGuard_DCM-1' to configure.");

    // We can blink the LED to show the provisioning web server is active
    gpio_config_t output_config = {
        .pin_bit_mask = (1ULL << STATUS_LED_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&output_config);


    while(1) {
        gpio_set_level(STATUS_LED_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_set_level(STATUS_LED_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
