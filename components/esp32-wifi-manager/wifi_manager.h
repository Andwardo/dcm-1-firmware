#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"

#ifdef __cplusplus
extern "C" {
#endif

// Event bit indicating STA is connected
#define WIFI_MANAGER_STA_CONNECTED_BIT    BIT0

// Message types used by the Wi-Fi Manager task
typedef enum {
    WIFI_MANAGER_MSG_CONNECT_STA,
    WIFI_MANAGER_MSG_START_PROVISIONING
} wifi_manager_message_e;

// Message struct passed to the Wi-Fi Manager task
typedef struct {
    int msg_id;
    wifi_config_t sta_config;
} wifi_manager_message_t;

// Initialize the Wi-Fi manager
void wifi_manager_init(void);

// Send a message to the Wi-Fi manager task
BaseType_t wifi_manager_send_message(void *msg);

// Retrieve the event group for Wi-Fi events
EventGroupHandle_t wifi_manager_get_event_group(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
