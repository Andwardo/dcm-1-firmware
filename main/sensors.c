/*
 * File:       sensors.c
 * Description: Stub implementations for sensor and status functions with logging
 *
 * Created on: 2025-06-21
 * Edited on:  2025-06-25
 *
 * Version: v8.2.43D – add logging to stub sensors
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#include "sensors.h"
#include "esp_log.h"

static const char *TAG = "SENSORS";

float get_temperature(void) {
    // TODO: replace with real sensor reading
    float temp = 25.0f;
    ESP_LOGI(TAG, "get_temperature -> %.2f°C", temp);
    return temp;
}

float get_humidity(void) {
    // TODO: replace with real sensor reading
    float hum = 50.0f;
    ESP_LOGI(TAG, "get_humidity -> %.2f%%", hum);
    return hum;
}

bool power_on(void) {
    // TODO: replace with real power sensor
    bool on = true;
    ESP_LOGI(TAG, "power_on -> %s", on ? "true" : "false");
    return on;
}

bool water_on(void) {
    // TODO: replace with real water sensor
    bool on = false;
    ESP_LOGI(TAG, "water_on -> %s", on ? "true" : "false");
    return on;
}

bool pads_on(void) {
    // TODO: replace with real pads sensor
    bool on = false;
    ESP_LOGI(TAG, "pads_on -> %s", on ? "true" : "false");
    return on;
}
