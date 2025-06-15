/* File: components/app_logic/include/app_logic.h
 *
 * Created on: 2025-06-11
 * Edited on:  2025-06-15 14:50:00 CDT
 * Version: v7.8.1
 * Author: R. Andrew Ballard (c) 2025
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the application logic FreeRTOS task.
 *
 * This creates and starts the `app_task` that runs the main loop.
 */
void app_logic_start_task(void);

#ifdef __cplusplus
}
#endif
