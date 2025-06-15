/*
 * File: main/main.c
 *
 * Created on: 2025-06-15
 * Edited on:  2025-06-15 (CDT)
 *
 * Version: v8.0.1
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#include <stdio.h>
#include "esp_log.h"

#include "board_manager.h"
#include "wifi_manager.h"
#include "app_logic.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing board manager...");
    board_manager_init();

    ESP_LOGI(TAG, "Initializing Wi-Fi manager...");
    wifi_manager_init();

    ESP_LOGI(TAG, "Starting Wi-Fi manager; application task will launch on connect...");
    // Pass the application task start function as callback
    wifi_manager_start(app_logic_start_task);

    // From here, app_logic_start_task will be invoked once Wi-Fi is connected.
}

