/*
 * File: components/board_manager/board_manager.c
 *
 * Created on: 12 June 2025 23:30:00
 * Last edited on: 12 June 2025 23:30:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
#include "board_manager.h"
#include "esp_log.h"

static const char *TAG = "BOARD_MANAGER";

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

bool board_manager_is_reset_button_pressed(void)
{
    return gpio_get_level(WIFI_RESET_PIN) == 0;
}
