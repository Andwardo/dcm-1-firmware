/*
 * File: components/board_manager/include/board_manager.h
 *
 * Created on: 12 June 2025 23:30:00
 * Last edited on: 12 June 2025 23:30:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */
#ifndef BOARD_MANAGER_H_
#define BOARD_MANAGER_H_

#include <stdbool.h>
#include "driver/gpio.h"

#define STATUS_LED_PIN GPIO_NUM_2
#define WIFI_RESET_PIN GPIO_NUM_4

esp_err_t board_manager_init(void);
bool board_manager_is_reset_button_pressed(void);

#endif /* BOARD_MANAGER_H_ */
