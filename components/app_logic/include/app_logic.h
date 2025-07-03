/*
 * File:    components/app_logic/include/app_logic.h
 * Created on: 2025-06-15
 * Edited on:  2025-07-03
 * Version: v8.3.2
 * Author:  R. Andrew Ballard (c) 2025
 */

#include "app_logic.h"

#ifndef APP_LOGIC_H_
#define APP_LOGIC_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Launches the main application logic task.
 *
 * This will create the FreeRTOS task that waits for Wi-Fi,
 * reads your board sensors every 5 s, builds a JSON payload,
 * and (eventually) publishes it via MQTT.
 */
void app_logic_run(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_LOGIC_H_ */
