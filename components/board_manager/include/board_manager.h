/*
 * File: components/board_manager/include/board_manager.h
 *
 * Created on: 12 June 2025 23:30:00
 * Last edited on: 15 June 2025 13:45:00 CDT
 *
 * Version: 8.1.1
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef BOARD_MANAGER_H_
#define BOARD_MANAGER_H_

#include <stdbool.h>
#include "driver/gpio.h"
#include "esp_err.h"

#define STATUS_LED_PIN GPIO_NUM_2
#define WIFI_RESET_PIN GPIO_NUM_4

/**
 * @brief Initialize all GPIOs and peripherals.
 */
esp_err_t board_manager_init(void);

/**
 * @brief Return true if reset button is physically pressed.
 */
bool board_manager_is_reset_button_pressed(void);

/**
 * @brief If reset button is held at boot > 3s, clear NVS and reboot.
 * 
 * Call this early in boot, before wifi_manager_start().
 */
void board_manager_check_reset_button_hold(void);

#endif /* BOARD_MANAGER_H_ */
