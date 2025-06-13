/*
 * File: components/app_logic/include/app_logic.h
 *
 * Created on: 13 June 2025 10:00:00
 * Last edited on: 13 June 2025 10:00:00
 *
 * Version: 8.0.0
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#ifndef APP_LOGIC_H
#define APP_LOGIC_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

/**
 * @brief Main application task that handles reset logic, networking, and sensor loop.
 */
void app_logic_task(void *param);

#endif // APP_LOGIC_H
