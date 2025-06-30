/*
Copyright (c) 2019 Tony Pottier

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

@file dns_server.c
@brief Defines an extremely basic DNS server for captive portal functionality.
*/

#include <lwip/sockets.h>
#include <string.h>
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <lwip/err.h>
#include <lwip/sys.h>
#include <lwip/netdb.h>
#include <lwip/dns.h>

#include "wifi_manager.h"
#include "dns_server.h"

// ✅ Define missing symbols (fallback/defaults)
#define WIFI_MANAGER_TASK_PRIORITY 4
#define DEFAULT_AP_IP "192.168.4.1"

// ✅ Forward declaration if not provided in wifi_manager.h
esp_netif_t* wifi_manager_get_esp_netif_sta(void);

static const char TAG[] = "dns_server";
static TaskHandle_t task_dns_server = NULL;
int socket_fd;

void dns_server_start() {
    if(task_dns_server == NULL){
        xTaskCreate(&dns_server, "dns_server", 3072, NULL, WIFI_MANAGER_TASK_PRIORITY - 1, &task_dns_server);
    }
}

void dns_server_stop(){
    if(task_dns_server){
        vTaskDelete(task_dns_server);
        close(socket_fd);
        task_dns_server = NULL;
    }
}

void dns_server(void *pvParameters) {

    struct sockaddr_in ra;
    ip4_addr_t ip_resolved;
    inet_pton(AF_INET, DEFAULT_AP_IP, &ip_resolved);

    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0){
        ESP_LOGE(TAG, "Failed to create socket");
        exit(0);
    }

    esp_netif_ip_info_t ip;
    esp_netif_t* netif_sta = wifi_manager_get_esp_netif_sta();
    ESP_ERROR_CHECK(esp_netif_get_ip_info(netif_sta, &ip));
    ra.sin_family = AF_INET;
    ra.sin_addr.s_addr = ip.ip.addr;
    ra.sin_port = htons(53);
    if (bind(socket_fd, (struct sockaddr *)&ra, sizeof(struct sockaddr_in)) == -1) {
        ESP_LOGE(TAG, "Failed to bind to 53/udp");
        close(socket_fd);
        exit(1);
    }

    struct sockaddr_in client;
    socklen_t client_len = sizeof(client);
    int length;
    uint8_t data[DNS_QUERY_MAX_SIZE];
    uint8_t response[DNS_ANSWER_MAX_SIZE];
    char ip_address[INET_ADDRSTRLEN];
    char *domain;
    int err;

    ESP_LOGI(TAG, "DNS Server listening on 53/udp");

    for(;;) {
        memset(data, 0x00, sizeof(data));
        length = recvfrom(socket_fd, data, sizeof(data), 0, (struct sockaddr *)&client, &client_len);

        if (length > 0 && ((length + sizeof(dns_answer_t) - 1) < DNS_ANSWER_MAX_SIZE)) {
            data[length] = '\0';

            memcpy(response, data, sizeof(dns_header_t));
            dns_header_t *dns_header = (dns_header_t*)response;
            dns_header->QR = 1;
            dns_header->OPCode = DNS_OPCODE_QUERY;
            dns_header->AA = 1;
            dns_header->RCode = DNS_REPLY_CODE_NO_ERROR;
            dns_header->TC = 0;
            dns_header->RD = 0;
            dns_header->ANCount = dns_header->QDCount;
            dns_header->NSCount = 0x0000;
            dns_header->ARCount = 0x0000;

            memcpy(response + sizeof(dns_header_t), data + sizeof(dns_header_t), length - sizeof(dns_header_t));

            inet_ntop(AF_INET, &(client.sin_addr), ip_address, INET_ADDRSTRLEN);
            domain = (char*) &data[sizeof(dns_header_t) + 1];
            for(char* c = domain; *c != '\0'; c++){
                if(*c < ' ' || *c > 'z') *c = '.';
            }
            ESP_LOGI(TAG, "Replying to DNS request for %s from %s", domain, ip_address);

            dns_answer_t *dns_answer = (dns_answer_t*)&response[length];
            dns_answer->NAME = __builtin_bswap16(0xC00C);
            dns_answer->TYPE = __builtin_bswap16(DNS_ANSWER_TYPE_A);
            dns_answer->CLASS = __builtin_bswap16(DNS_ANSWER_CLASS_IN);
            dns_answer->TTL = 0x00000000;
            dns_answer->RDLENGTH = __builtin_bswap16(0x0004);
            dns_answer->RDATA = ip_resolved.addr;

            err = sendto(socket_fd, response, length + sizeof(dns_answer_t), 0, (struct sockaddr *)&client, client_len);
            if (err < 0) {
                ESP_LOGE(TAG, "UDP sendto failed: %d", err);
            }
        }

        taskYIELD();
    }

    close(socket_fd);
    vTaskDelete(NULL);
}
