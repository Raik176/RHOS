//
// Created by RHM on 01/05/2025.
//

// from https://git.savannah.gnu.org/cgit/grub.git/tree/doc/multiboot2.h?h=multiboot2
// fetched on the 1st of May 2025

#ifndef RHOS_MULTIBOOT_H
#define RHOS_MULTIBOOT_H

#include "kstdlib.h"

#define MULTIBOOT_TAG_TYPE_END              0
#define MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME 2
#define MULTIBOOT_TAG_TYPE_MMAP             6
#define MULTIBOOT_TAG_TYPE_ACPI_OLD         14

struct __attribute__((aligned(8))) multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct __attribute__((aligned(8))) multiboot_tag_string {
    uint32_t type;
    uint32_t size;
    char string[];
};

struct multiboot_mmap_entry {
    uint64_t addr;
    uint64_t len;
#define MULTIBOOT_MEMORY_AVAILABLE              1
#define MULTIBOOT_MEMORY_RESERVED               2
#define MULTIBOOT_MEMORY_ACPI_RECLAIMABLE       3
#define MULTIBOOT_MEMORY_NVS                    4
#define MULTIBOOT_MEMORY_BADRAM                 5
    uint32_t type;
    uint32_t zero;
};
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct __attribute__((aligned(8))) multiboot_tag_mmap {
    uint32_t type;
    uint32_t size;
    uint32_t entry_size;
    uint32_t entry_version;
    struct multiboot_mmap_entry entries[];
};

struct multiboot_info {
    uint32_t total_size;
    uint32_t reserved;

    struct multiboot_tag tags[];
};

#endif //RHOS_MULTIBOOT_H
