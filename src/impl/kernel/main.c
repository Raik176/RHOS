//
// Created by RHM on 30/04/2025.
//

#include "kernel/print.h"
#include "kernel/multiboot.h"
#include "kernel/keyboard.h"
#include "kernel/utility.h"
#include "kernel/acpi.h"
#include "kernel/timer/timer.h"
#include "x86_64/paging.h"
#include "x86_64/interrupts.h"

__attribute__((used))
void kmain(uint64_t addr);

__attribute__((used))
void kmain(uint64_t addr) {
    kcls();
    kset_attr(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);

    init_idt();

    struct multiboot_tag *tag = (struct multiboot_tag *) (addr + 8);
    struct multiboot_tag_mmap *memory_map = NULL;
    struct multiboot_tag_acpi *acpi_info = NULL;

    while (tag->type != MULTIBOOT_TAG_TYPE_END) {
        switch (tag->type) {
            case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME: {
                kprintf("Booted from boot loader: %s\n",
                        ((struct multiboot_tag_string *) tag)->string);
                break;
            }
            case MULTIBOOT_TAG_TYPE_MMAP: {
                memory_map = (struct multiboot_tag_mmap *) tag;
                break;
            }
            case MULTIBOOT_TAG_TYPE_ACPI_OLD:
            case MULTIBOOT_TAG_TYPE_ACPI_NEW: {
                acpi_info = (struct multiboot_tag_acpi *) tag;
                break;
            }
        }

        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~(uint32_t) 7));
    }

    if (acpi_info == NULL) {
        panic("Could not find ACPI info.");
    }
    init_acpi((uintptr_t) acpi_info->rsdp);

    if (memory_map == NULL) {
        panic("Could not find a memory map.");
    }
    init_paging(memory_map);
    if (!is_paging_initialized()) {
        panic("Paging was unable to successfully initialize.");
    }

    kprintf("%p\n", get_page_table());


    fetch_acpi_data();

    init_timer();
    init_keyboard();

    for (;;);
}