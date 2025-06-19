/*
 * File: components/board_manager/board_manager.c
 * Description: Manages hardware-specific interactions by reading GPIOs.
 *
 * Created on: 2025-06-18
 * Edited on:  2025-06-19
 *
 * Version: v8.3.2
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#include "board_manager.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "BOARD_MANAGER";

// --- FINAL, SAFE GPIO Pin Definitions ---
#define GPIO_PADS_A         6
#define GPIO_WATER_A        7
#define GPIO_POWER_A        12
#define GPIO_PADS_B         18
#define GPIO_WATER_B        8  // <-- Moved from GPIO 19
#define GPIO_POWER_B        9  // <-- Moved from GPIO 20

// --- Public API Implementation ---

esp_err_t board_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Board Manager with final pinout...");

    uint64_t pin_bit_mask = (1ULL << GPIO_PADS_A)  | (1ULL << GPIO_PADS_B) |
                            (1ULL << GPIO_WATER_A) | (1ULL << GPIO_WATER_B)|
                            (1ULL << GPIO_POWER_A) | (1ULL << GPIO_POWER_B);

    gpio_config_t io_conf = {
        .pin_bit_mask = pin_bit_mask,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t err = gpio_config(&io_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIOs: %s", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Board Manager initialized successfully.");
    return ESP_OK;
}

esp_err_t board_manager_get_status(dcm_status_t *status) {
    if (status == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    // Read the digital output from the comparators for each line
    bool pads_a_state = gpio_get_level(GPIO_PADS_A);
    bool pads_b_state = gpio_get_level(GPIO_PADS_B);

    bool water_a_state = gpio_get_level(GPIO_WATER_A);
    bool water_b_state = gpio_get_level(GPIO_WATER_B);
    
    bool power_a_state = gpio_get_level(GPIO_POWER_A);
    bool power_b_state = gpio_get_level(GPIO_POWER_B);

    // The alert is active if EITHER of the A or B lines is triggered by its comparator.
    status->pads_worn = pads_a_state || pads_b_state;
    status->water_low = water_a_state || water_b_state;
    status->power_ok  = power_a_state || power_b_state;

    return ESP_OK;
}
