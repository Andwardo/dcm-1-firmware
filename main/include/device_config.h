#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <esp_wifi.h>

#define DEVICE_ID       "PianoGuard001"
/* Bump this on every refactor by 0.0.1 */
#define FW_VERSION      "8.2.46"
/* The MQTT topic we publish status to: */
#define MQTT_TOPIC_STATUS  "pianoguard/status"

static inline const char* wifi_sta_ssid(void)
{
    static wifi_ap_record_t ap_info;
    if (esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
        return (const char*)ap_info.ssid;
    }
    return "<unknown>";
}

#endif // DEVICE_CONFIG_H
