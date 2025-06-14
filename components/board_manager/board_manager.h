/*
 * File: components/board_manager/include/board_manager.h
 *
 * Created on: 13 June 2025 09:05:00
 * Last edited on: 13 June 2025 14:00:00
 *
 * Version: 8.1.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef BOARD_MANAGER_H
#define BOARD_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Initializes the board's GPIO pins for LED and buttons.
 *
 * @return esp_err_t ESP_OK on success, or an error code on failure.
 */
esp_err_t board_manager_init(void);

/**
 * @brief Sets the state of the onboard status LED.
 *
 * @param on true to turn the LED on, false to turn it off.
 */
void board_manager_set_led_state(bool on);

/**
 * @brief Reads the current state of the factory reset button.
 *
 * @return true if the button is pressed, false otherwise.
 */
bool board_manager_get_factory_reset_button_state(void);

#endif // BOARD_MANAGER_H
