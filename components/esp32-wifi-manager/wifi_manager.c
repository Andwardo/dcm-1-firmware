/*
 * File: components/esp32-wifi-manager/wifi_manager.c
 *
 * This is the main logic for the esp32-wifi-manager library.
 *
 * Version: 7.1.2
 *
 * Author: R. Andrew Ballard (c) 2025
 *
 */

#include "wifi_manager.h"
#include <string.h>
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "freertos/event_groups.h"
#include "esp_http_server.h"
#include "esp_mac.h"

static const char TAG[] = "wifi_manager";

/* Prototypes */
void http_app_start(void);
static void wifi_manager_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

void wifi_manager_start() {
    ESP_LOGI(TAG, "Starting Wi-Fi Manager");

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_manager_event_handler, NULL, NULL));

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "PianoGuard_DCM-1",
            .ssid_len = strlen("PianoGuard_DCM-1"),
            .password = "",
            .max_connection = 4,
            .authmode = WIFI_AUTH_OPEN
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wi-Fi AP started. SSID: PianoGuard_DCM-1");
    http_app_start();
}

// Event handler for Wi-Fi events
static void wifi_manager_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data){
	if (event_id == WIFI_EVENT_AP_STACONNECTED) {
		ESP_LOGI(TAG, "A station connected to the AP.");
	} else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
		ESP_LOGI(TAG, "A station disconnected from the AP.");
	}
}

