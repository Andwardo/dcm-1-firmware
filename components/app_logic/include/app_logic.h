/*
 * File: components/app_logic/include/app_logic.h
 *
 * Created on: 2025-06-15
 * Edited on:  2025-06-18
 *
 * Version: v8.2.0
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef APP_LOGIC_H_
#define APP_LOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initializes the main application logic component by creating its FreeRTOS task.
 */
void app_logic_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_LOGIC_H_ */
