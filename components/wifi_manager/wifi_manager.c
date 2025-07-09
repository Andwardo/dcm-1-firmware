/**
 * File: wifi_manager.c
 * Description: Wi-Fi manager implementation for captive portal, NVS, and event handling.
 * Created on: 2025-06-18
 * Edited on:  2025-07-09
 * Version: v8.6.2
 * Author: R. Andrew Ballard (c) 2025
 * Add missing header includes required by ESP-IDF v5.2.5 to fix build errors.
 **/

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

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
}