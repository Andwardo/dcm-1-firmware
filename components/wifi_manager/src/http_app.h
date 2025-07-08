/*
 *  http_app.h
 *
 *  Created on: 2025-07-07
 *      Author: Andwardo
 *      Version: v1.3
 */


#ifndef HTTP_APP_H
#define HTTP_APP_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Starts the embedded HTTP server to serve captive portal assets.
 *
 * This initializes the HTTP server and sets up handlers for static files like
 * style.css and code.js which are embedded via binary linking.
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL otherwise
 */
httpd_handle_t start_http_server(void);

#ifdef __cplusplus
}
#endif

#endif // HTTP_APP_H