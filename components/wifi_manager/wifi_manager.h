/*
Copyright (c) 2017-2025 Tony Pottier & R. Andrew Ballard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction...
*/

#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event bit to signal the station (STA) is connected
#define WIFI_MANAGER_STA_CONNECTED_BIT  BIT0

// Messages for Wi-Fi Manager task operations
typedef enum {
    WIFI_MANAGER_MSG_CONNECT_STA,        // Connect in station mode with provided credentials
    WIFI_MANAGER_MSG_START_PROVISIONING  // Start SoftAP and captive-portal provisioning
} wifi_manager_message_e;

// Structure to pass messages into the Wi-Fi Manager task
typedef struct {
    wifi_manager_message_e msg_id;  // Message type
    wifi_config_t          sta_config;
} wifi_manager_message_t;

/**
 * @brief Initialize Wi-Fi Manager internals: queue, event group, netif, handlers.
 */
void wifi_manager_init(void);

/**
 * @brief Initialize internals and immediately start provisioning mode (SoftAP + portal).
 */
void wifi_manager_start(void);

/**
 * @brief Enqueue a message to the Wi-Fi Manager task.
 *
 * @param msg Pointer to message struct (type + optional STA config).
 * @return pdTRUE if queued successfully, pdFALSE otherwise.
 */
BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg);

/**
 * @brief Retrieve the event group handle for STA connection bits.
 *
 * Used to wait for WIFI_MANAGER_STA_CONNECTED_BIT.
 */
EventGroupHandle_t wifi_manager_get_event_group(void);

/**
 * @brief Retrieve the esp_netif handle for the STA interface.
 *
 * Useful for DNS responder to query the AP interface.
 */
esp_netif_t *wifi_manager_get_esp_netif_sta(void);

/**
 * @brief Trigger Wi-Fi Manager to connect using saved credentials.
 *
 * Called by HTTP portal or boot logic.
 */
void wifi_manager_connect_sta(const char *ssid, const char *password);

/**
 * @brief Check if saved credentials exist in NVS.
 */
bool wifi_credentials_exist(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
