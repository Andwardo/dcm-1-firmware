/**
 * File: main.c
 * Description: Main entry point for the PianoGuard DCM-1 application.
 * Created on: 2025-06-25
 * Edited on:  2025-07-09
 * Version: v8.6.2
 * Author: R. Andrew Ballard (c) 2025
 * Fix: Add stdint.h for uint16_t errors and terminate app_main() properly.
 **/

#include <stdio.h>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"

void app_main(void) {
    ESP_LOGI("main", "PianoGuard DCM-1 starting up...");

    wifi_manager_start();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}