#ifndef WIFI_MANAGER_H_INCLUDED
#define WIFI_MANAGER_H_INCLUDED

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_netif.h"

#ifdef __cplusplus
extern "C" {
#endif

bool wifi_credentials_exist(void);

// Event bit to signal the station (STA) is connected
#define WIFI_MANAGER_STA_CONNECTED_BIT  BIT0

// Messages for Wi-Fi Manager task operations
typedef enum {
    WIFI_MANAGER_MSG_CONNECT_STA,
    WIFI_MANAGER_MSG_START_PROVISIONING
} wifi_manager_message_e;

// Structure to pass messages into the Wi-Fi Manager task
typedef struct {
    wifi_manager_message_e msg_id;
    wifi_config_t          sta_config;
} wifi_manager_message_t;

// New: declare function called from http_app
void wifi_manager_connect_sta(const char *ssid, const char *password);

void wifi_manager_init(void);
void wifi_manager_start(void);
BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg);
EventGroupHandle_t wifi_manager_get_event_group(void);
esp_netif_t *wifi_manager_get_esp_netif_sta(void);

#ifdef __cplusplus
}
#endif

#endif /* WIFI_MANAGER_H_INCLUDED */
