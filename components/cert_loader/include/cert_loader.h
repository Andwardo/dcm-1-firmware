/*
 * Project:   PianoGuard_DCM-1
 * File:      main/include/cert_loader.h
 * Version:   1.0.0
 * Author:    R. Andrew Ballard
 * Date:      June 23, 2025
 *
 * Header for the module that loads certificates from a dedicated partition.
 */
#ifndef CERT_LOADER_H
#define CERT_LOADER_H

#include "esp_err.h"

extern const char* server_root_ca_pem;
extern const char* client_cert_pem;
extern const char* client_key_pem;

esp_err_t load_certificates_from_flash(void);

#endif // CERT_LOADER_H
