//
// Created by RHM on 12/05/2025.
//

#include "kernel/acpi.h"
#include "kernel/utility.h"
#include "kernel/print.h"
#include "x86_64/paging.h"

static acpi_info_t g_acpi_data;
static uintptr_t g_root_sdt_phys = 0;

static bool validate_acpi_checksum(acpi_std_header_t *table_header) {
    uint8_t sum = 0;
    for (uint32_t i = 0; i < table_header->len; i++) {
        sum += ((uint8_t *)table_header)[i];
    }
    return sum == 0;
}

void init_acpi(uintptr_t rsdp_addr) {
    memset(&g_acpi_data, 0, sizeof(acpi_info_t));

    acpi_rsdp_descriptor_t* rsdp = (acpi_rsdp_descriptor_t *) rsdp_addr;
    if (!rsdp) {
        panic("ACPI: Failed to find RSDP!");
    }

    if (strncmp(rsdp->signature, "RSD PTR ", 8) != 0) {
        panic("ACPI: Invalid RSDP signature!");
    }

    uint8_t checksum_v1 = 0;
    for (size_t i = 0; i < offsetof(acpi_rsdp_descriptor_t , len); ++i) {
        checksum_v1 += ((uint8_t*)rsdp)[i];
    }
    if (checksum_v1 != 0) {
        panic("ACPI: RSDP V1 checksum failed!");
    }

    g_acpi_data.acpi_revision = rsdp->rev;

    if (rsdp->rev >= 2) {
        uint8_t checksum_v2 = 0;
        for (uint32_t i = 0; i < rsdp->len; ++i) {
            checksum_v2 += ((uint8_t*)rsdp)[i];
        }
        if (checksum_v2 != 0) {
            kprintf("ACPI: RSDP V2 checksum failed! Falling back to RSDT if possible.");
            g_acpi_data.acpi_revision = 1;
            g_root_sdt_phys = rsdp->rsdt_addr;
        } else {
            kprintf("ACPI: Using ACPI Revision %d (XSDT).", rsdp->rev);
            g_root_sdt_phys = rsdp->xsdt_addr;
        }
    } else {
        kprintf("ACPI: Using ACPI Revision 1 (RSDT).");
        g_root_sdt_phys = rsdp->rsdt_addr;
    }
    kprintf(" Using STD addr: %p.\n", g_root_sdt_phys);
}

void fetch_acpi_data(void) {
    acpi_std_header_t* root_header = (acpi_std_header_t *) g_root_sdt_phys;
    kprintf("Hello!\n");
    map_range(get_page_table(), g_root_sdt_phys,g_root_sdt_phys,
              sizeof(acpi_std_header_t), PTE_PRESENT);
    kprintf("Test\n");

    if (!root_header) {
        panic("ACPI: Failed to obtain Root SDT header!");
    }
    kprintf("Test: len; %llu", root_header->len);
    panic("");
    /*
    if (!validate_acpi_checksum(root_header)) {
        panic("ACPI: Root STD checksum failed!");
    }
*/

}

const acpi_info_t* get_acpi_info(void) {
    return NULL;
}