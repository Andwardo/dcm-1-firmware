/*
 *  http_app.h
 *
 *  Created on: 2025-06-18
 *  Edited on: 2025-07-08
 *      Author: Andwardo
 *      Version: v8.2.46
 */

#ifndef HTTP_APP_H_
#define HTTP_APP_H_

#include "esp_err.h"
#include "esp_http_server.h"

httpd_handle_t start_http_server(void);

#endif /* HTTP_APP_H_ */