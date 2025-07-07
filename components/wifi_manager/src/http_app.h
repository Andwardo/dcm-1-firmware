/*
 *  http_app.h
 *
 *  Created on: 2025-07-07
 *      Author: Andwardo
 *      Version: v1.1
 */

#ifndef HTTP_APP_H_
#define HTTP_APP_H_

#include "esp_err.h"

/**
 * @brief Start the HTTP server with captive portal file handlers.
 *
 * This function sets up routes for serving static files (e.g., index.html, code.js)
 * during captive portal mode.
 *
 * @return
 *      - ESP_OK on success
 *      - appropriate ESP_ERR_* code on failure
 */
esp_err_t http_app_start(void);

#endif /* HTTP_APP_H_ */