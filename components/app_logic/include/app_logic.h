/* File: components/app_logic/include/app_logic.h
 *
 -* Version: v7.8.1
 +* Version: v8.0.6
 */

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

void app_logic_init(void);

/**
 * @brief Start the application logic FreeRTOS task.
 */
void app_logic_start_task(void);

#ifdef __cplusplus
}
#endif
