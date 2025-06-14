/*
 * File: components/wifi_manager/wifi_manager.c
 *
 * Created on: 13 June 2025 11:10:00
 * Last edited on: 14 June 2025 10:20:00
 *
 * Version: 8.0.3
 */

#include "wifi_manager.h"
#include "provisioning_manager.h"
#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_http.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"

static const char *TAG = "wifi_mgr";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_PROV_EVENT && event_id == WIFI_PROV_CRED_RECV) {
        char ssid[33], pass[65];
        provisioning_manager_get_credentials(ssid, pass);
        wifi_config_t cfg = {
            .sta = {
                .ssid = {0},
                .password = {0},
            }
        };
        strncpy((char*)cfg.sta.ssid, ssid, sizeof(cfg.sta.ssid));
        strncpy((char*)cfg.sta.password, pass, sizeof(cfg.sta.password));
        ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg));
        esp_wifi_connect();
    }
}

esp_err_t wifi_manager_start(void)
{
    ESP_LOGI(TAG, "Initializing Wi-Fi manager");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                              &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_PROV_EVENT, ESP_EVENT_ANY_ID,
                                              &wifi_event_handler, NULL));

    // if no saved credentials, run provisioning
    provisioning_manager_init();

    return ESP_OK;
}

esp_err_t wifi_manager_stop(void)
{
    ESP_LOGI(TAG, "Stopping Wi-Fi");
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler));
    return ESP_OK;
}
