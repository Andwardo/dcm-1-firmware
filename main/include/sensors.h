/*
 * File: sensors.h
 * Description: Sensor and status function declarations
 *
 * Created on: 2025-06-21
 * Edited on:  2025-06-21
 *
 * Version: v8.2.38
 *
 * Author: R. Andrew Ballard (c) 2025
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <stdbool.h>

float get_temperature(void);
float get_humidity(void);
bool  power_on(void);
bool  water_on(void);
bool  pads_on(void);

#endif // SENSORS_H
