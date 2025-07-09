/**
 * File: wifi_manager.c
 * Description: Wi-Fi manager implementation for captive portal, NVS, and event handling.
 * Created on: 2025-06-18
 * Edited on:  2025-07-09
 * Version: v8.6.6
 * Author: R. Andrew Ballard (c) 2025
 * Fix: Add NVS initialization required before esp_wifi_init() on ESP-IDF v5.2.5.
 **/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include <string.h>

#include "wifi_manager.h"

static const char TAG[] = "wifi_manager";

static EventGroupHandle_t wifi_event_group;

static char ap_ssid[32] = {0};

EventGroupHandle_t wifi_manager_get_event_group(void) {
	return wifi_event_group;
}

void generate_ap_ssid_from_mac(void) {
	uint8_t mac[6];
	esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
	snprintf(ap_ssid, sizeof(ap_ssid), "PianoGuard-%02X%02X", mac[4], mac[5]);
}

const char* get_ap_ssid(void) {
	return ap_ssid;
}

void wifi_manager_start(void) {
	wifi_event_group = xEventGroupCreate();
	generate_ap_ssid_from_mac();

	ESP_LOGI(TAG, "Starting Wi-Fi manager, AP SSID: %s", ap_ssid);

	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);

	ESP_ERROR_CHECK(esp_netif_init());
	ESP_ERROR_CHECK(esp_event_loop_create_default());
	esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));

	wifi_config_t ap_config = {
		.ap = {
			.ssid = {0},
			.ssid_len = 0,
			.channel = 1,
			.password = "",
			.max_connection = 4,
			.authmode = WIFI_AUTH_OPEN
		},
	};
	strncpy((char *)ap_config.ap.ssid, ap_ssid, sizeof(ap_config.ap.ssid) - 1);

	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	ESP_ERROR_CHECK(esp_wifi_start());

	ESP_LOGI(TAG, "SoftAP started with SSID: %s", ap_ssid);
}