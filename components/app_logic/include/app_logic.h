/*
 * File:    app_logic.h
 * Created: 2025-06-15
 * Version: v8.2.1
 * Author:  R. Andrew Ballard (c) 2025
 */

#ifndef APP_LOGIC_H_
#define APP_LOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "freertos/FreeRTOS.h"
#include "esp_log.h"

void app_logic_init(void);
void app_logic_run(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_LOGIC_H_ */
