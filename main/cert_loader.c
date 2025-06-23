/*
 * Project:   PianoGuard_DCM-1
 * File:      main/cert_loader.c
 * Version:   1.0.0
 * Author:    R. Andrew Ballard
 * Date:      June 23, 2025
 *
 * This module does not actually load from a partition. It provides pointers
 * to certificate data that is embedded directly into the firmware binary.
 */
#include "cert_loader.h"
#include "esp_log.h"

static const char *TAG = "cert_loader";

// These 'asm' symbols are created by the build system from the target_add_binary_data command.
extern const uint8_t root_ca_pem_start[]          asm("_binary_root_ca_pem_start");
extern const uint8_t client_crt_start[]           asm("_binary_client_crt_start");
extern const uint8_t client_key_start[]           asm("_binary_client_key_start");

const char* server_root_ca_pem = NULL;
const char* client_cert_pem = NULL;
const char* client_key_pem = NULL;

// In this model, "loading" simply means assigning the pointers to the embedded data.
esp_err_t load_certificates_from_flash(void)
{
    ESP_LOGI(TAG, "Assigning pointers to embedded certificates...");
    server_root_ca_pem = (const char*)root_ca_pem_start;
    client_cert_pem = (const char*)client_crt_start;
    client_key_pem = (const char*)client_key_start;
    ESP_LOGI(TAG, "Certificate pointers assigned.");
    return ESP_OK;
}
