/*
 * Project:   PianoGuard_DCM-1
 * File:      main/cert_loader.c
 * Version:   1.0.1
 * Author:    R. Andrew Ballard
 * Date:      June 23, 2025
 * Edited:    June 24, 2025
 * Implements the logic for mounting the dedicated 'certs' SPIFFS partition
 * and loading device-specific certificates into memory for use by network clients.
 */
#include "cert_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "cert_loader";

// Define the actual global variables that will store the certificate data.
const char* server_root_ca_pem = NULL;
const char* client_cert_pem = NULL;
const char* client_key_pem = NULL;

// Helper function to read a file from the mounted SPIFFS partition into a new buffer.
static esp_err_t read_file_from_spiffs(const char* path, char** out_buffer)
{
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for reading: %s", path);
        return ESP_FAIL;
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    if (buffer == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for file buffer");
        fclose(f);
        return ESP_ERR_NO_MEM;
    }

    fread(buffer, 1, size, f);
    fclose(f);
    buffer[size] = '\0'; // Ensure the buffer is null-terminated

    *out_buffer = buffer;
    return ESP_OK;
}

esp_err_t load_certificates_from_flash(void)
{
    ESP_LOGI(TAG, "Mounting 'certs' partition...");
    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/certs",
        .partition_label = "certs",
        .max_files = 3, // Root CA, Client Cert, Client Key
        .format_if_mount_failed = false // Never format a readonly partition
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        // ESP_ERR_INVALID_STATE means it's already mounted, which is okay.
        if (ret == ESP_ERR_INVALID_STATE) {
            ESP_LOGW(TAG, "'certs' partition already mounted.");
        } else {
            ESP_LOGE(TAG, "Failed to mount 'certs' partition: %s", esp_err_to_name(ret));
            return ret;
        }
    }

    // NOTE: This assumes your files in the 'certs' directory are named exactly this way.
    if (read_file_from_spiffs("/certs/root_ca.pem", (char**)&server_root_ca_pem) != ESP_OK) return ESP_FAIL;
    if (read_file_from_spiffs("/certs/client.crt", (char**)&client_cert_pem) != ESP_OK) return ESP_FAIL;
    if (read_file_from_spiffs("/certs/client.key", (char**)&client_key_pem) != ESP_OK) return ESP_FAIL;

    ESP_LOGI(TAG, "All certificates have been loaded from flash into memory.");

    // We can unmount the partition now that the files are in memory to save resources.
    esp_vfs_spiffs_unregister(conf.partition_label);
    ESP_LOGI(TAG, "'certs' partition unmounted.");

    return ESP_OK;
}
