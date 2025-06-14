/*
 * File: components/provisioning_manager/provisioning_manager.c
 *
 * Created on: 14 June 2025 10:05:00
 * Last edited on: 14 June 2025 10:10:00
 *
 * Version: 1.0
 */

#include "provisioning_manager.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_http.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "provision_mgr";

esp_err_t provisioning_manager_init(void)
{
    ESP_LOGI(TAG, "Starting Wi-Fi provisioning");
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_prov_mgr_config_t config = {
        .scheme = wifi_prov_scheme_http,
        .scheme_event_handler = WIFI_PROV_SCHEME_EVENT_HANDLER_HTTP
    };
    ESP_ERROR_CHECK(wifi_prov_mgr_init(config));
    ESP_ERROR_CHECK(wifi_prov_mgr_start_provisioning(
        WIFI_PROV_TRANSPORT_SOFTAP,
        NULL
    ));
    return ESP_OK;
}

esp_err_t provisioning_manager_get_credentials(char *ssid, char *password)
{
    wifi_config_t wc = { 0 };
    ESP_ERROR_CHECK(wifi_prov_mgr_get_provisioning_result(&wc));
    strncpy(ssid, (char *)wc.sta.ssid, 32);
    strncpy(password, (char *)wc.sta.password, 64);
    ESP_LOGI(TAG, "Received credentials SSID='%s'", ssid);
    ESP_ERROR_CHECK(wifi_prov_mgr_clear_provisioning());
    return ESP_OK;
}
