/*
 * File: components/board_manager/include/board_manager.h
 * Description: Manages hardware-specific interactions, including sensor GPIOs.
 *
 * Created on: 2025-06-18
 * Edited on:  2025-06-18
 *
 * Version: v8.3.0
 *
 * Author: R. Andrew Ballard (c) 2025
 */
#ifndef BOARD_MANAGER_H
#define BOARD_MANAGER_H

#include "esp_err.h"
#include <stdbool.h>

/**
 * @brief Structure to hold the final status of all DCM inputs.
 */
typedef struct {
    bool power_ok;
    bool water_low;
    bool pads_worn;
} dcm_status_t;

/**
 * @brief Initializes the board manager component by configuring GPIOs.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t board_manager_init(void);

/**
 * @brief Gets the current status of the DCM sensors by reading and combining GPIO states.
 * @param status Pointer to a dcm_status_t struct to be filled.
 * @return esp_err_t ESP_OK on success.
 */
esp_err_t board_manager_get_status(dcm_status_t *status);


#endif // BOARD_MANAGER_H
