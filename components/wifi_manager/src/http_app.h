/*
 *  http_app.h
 *
 *  Copyright (c) 2017-2025 Tony Pottier & R. Andrew Ballard
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
 */

#ifndef HTTP_APP_H_INCLUDED
#define HTTP_APP_H_INCLUDED

#include <stdbool.h>
#include <esp_http_server.h>

void http_app_start(bool lru_purge_enable);

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The URL where the captive-portal lives (default root). */
#define WEBAPP_LOCATION CONFIG_WEBAPP_LOCATION

/**
 * @brief Start the HTTP captive-portal.
 * @param lru_purge_enable  If true, purge least-recently-used cache entries.
 */
void http_app_start(bool lru_purge_enable);

/** @brief Stop the HTTP captive-portal. */
void http_app_stop(void);

/**
 * @brief Hook into the POST /connect handler.
 * @param method   HTTP method (e.g. HTTP_POST)
 * @param handler  Your handler to replace/augment internal wifi_connect_handler
 * @return ESP_OK or ESP_ERR_INVALID_ARG if unsupported
 */
esp_err_t http_app_set_handler_hook(httpd_method_t method,
                                    esp_err_t (*handler)(httpd_req_t *req));

/**
 * @brief Internal handler for POST /connect (Wi-Fi credentials submission).
 * If you need to override it, use http_app_set_handler_hook().
 */
esp_err_t wifi_connect_handler(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif /* HTTP_APP_H_INCLUDED */
