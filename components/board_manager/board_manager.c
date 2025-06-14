/*
 * File: components/board_manager/board_manager.c
 *
 * Created on: 13 June 2025 09:10:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.1.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "board_manager.h"
#include "driver/gpio.h"
#include "esp_log.h"

// --- IMPORTANT ---
// Replace these with the actual GPIO numbers for your hardware.
#define STATUS_LED_GPIO      GPIO_NUM_2
#define FACTORY_RESET_GPIO   GPIO_NUM_0

static const char *TAG = "BOARD_MGR";

esp_err_t board_manager_init(void) {
    ESP_LOGI(TAG, "Initializing board hardware...");

    // Configure Status LED
    gpio_config_t led_conf = {
        .pin_bit_mask = (1ULL << STATUS_LED_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    esp_err_t err = gpio_config(&led_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Status LED: %s", esp_err_to_name(err));
        return err;
    }

    // Configure Factory Reset Button
    gpio_config_t btn_conf = {
        .pin_bit_mask = (1ULL << FACTORY_RESET_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    err = gpio_config(&btn_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure Factory Reset Button: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Board hardware initialized successfully.");
    return ESP_OK;
}

void board_manager_set_led_state(bool on) {
    gpio_set_level(STATUS_LED_GPIO, on ? 1 : 0);
}

bool board_manager_get_factory_reset_button_state(void) {
    return gpio_get_level(FACTORY_RESET_GPIO) == 0;
}
