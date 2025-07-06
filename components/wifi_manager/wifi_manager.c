/**
 * wifi_manager.c
 *
 * Created on: 2025-06-25
 * Edited on: 2025-07-07
 * Author: R. Andrew Ballard (c) 2025
 * Version: v8.7.12
 */

#include "wifi_manager.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

static const char *TAG = "WIFI_MANAGER";

static QueueHandle_t wifi_manager_queue = NULL;

/**
 * @brief Send a message to the WiFi Manager queue
 */
BaseType_t wifi_manager_send_message(const wifi_manager_message_t *msg) {
    if (wifi_manager_queue == NULL) {
        ESP_LOGE(TAG, "wifi_manager_queue not initialized!");
        return pdFAIL;
    }
    return xQueueSend(wifi_manager_queue, msg, portMAX_DELAY);
}

/**
 * @brief WiFi manager event loop task
 */
static void wifi_manager_task(void *pvParameters) {
    wifi_manager_message_t msg;
    while (1) {
        if (xQueueReceive(wifi_manager_queue, &msg, portMAX_DELAY)) {
            switch (msg.msg_code) {
                case WIFI_MANAGER_MSG_START_AP:
                    ESP_LOGI(TAG, "Starting SoftAP provisioning");
                    // Implement SoftAP startup here
                    break;
                case WIFI_MANAGER_MSG_START_STA:
                    ESP_LOGI(TAG, "Starting STA connection");
                    // Implement STA connection here
                    break;
                default:
                    ESP_LOGW(TAG, "Unknown message code: %d", msg.msg_code);
                    break;
            }
        }
    }
}

/**
 * @brief Initialize Wi-Fi Manager
 */
void wifi_manager_init(void) {
    ESP_LOGI(TAG, "Initializing Wi-Fi Manager…");

    // Create event loop if not already created
    esp_err_t err = esp_event_loop_create_default();
    if (err != ESP_OK && err != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "esp_event_loop_create_default failed: 0x%x", err);
        return;
    }

    // Initialize queue
    wifi_manager_queue = xQueueCreate(10, sizeof(wifi_manager_message_t));
    if (!wifi_manager_queue) {
        ESP_LOGE(TAG, "Failed to create Wi-Fi manager queue");
        return;
    }

    // Start task
    xTaskCreate(wifi_manager_task, "wifi_manager_task", 4096, NULL, 5, NULL);
}

/**
 * @brief Launch SoftAP and HTTP provisioning
 */
void wifi_manager_start(void) {
    wifi_manager_message_t msg = {
        .msg_code = WIFI_MANAGER_MSG_START_AP
    };
    wifi_manager_send_message(&msg);
}
