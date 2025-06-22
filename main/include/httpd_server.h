/*
 * Project:    PianoGuard_DCM-1
 * File:       main/include/httpd_server.h
 * Version:    8.2.45
 * Author:     Andrew Ballard
 * Date:       Jun 25 2025
 */

#pragma once

#include "esp_err.h"

esp_err_t start_http_server(void);
esp_err_t stop_http_server(void);
