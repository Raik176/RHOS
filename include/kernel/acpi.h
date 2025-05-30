//
// Created by RHM on 12/05/2025.
//

#ifndef RHOS_ACPI_H
#define RHOS_ACPI_H

#include "kstdlib.h"

typedef struct __attribute__((packed)) {
    char signature[8];
    uint8_t checksum;
    char OEM_id[6];
    uint8_t rev;
    uint32_t rsdt_addr;

    uint32_t len;
    uint64_t xsdt_addr;
    uint8_t extended_checksum;
    uint8_t reserved[3];
} acpi_rsdp_descriptor_t;

typedef struct __attribute__((packed)) {
    char signature[4];
    uint32_t len;
    uint8_t rev;
    uint8_t checksum;
    char OEM_id[6];
    char OEM_table_id[8];
    uint32_t OEM_revision;
    uint32_t creator_id;
    uint32_t creator_rev;
} acpi_std_header_t;

typedef struct {
    bool initialized;
    uint8_t acpi_revision;

} acpi_info_t;

void init_acpi(uintptr_t rsdp_addr);
void fetch_acpi_data(void);

const acpi_info_t* get_acpi_info(void);

#endif //RHOS_ACPI_H
