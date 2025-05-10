//
// Created by RHM on 30/04/2025.
//

#include "print.h"
#include "interrupts.h"
#include "multiboot.h"
#include "keyboard.h"
#include "paging.h"
#include "utility.h"
#include "timer/timer.h"

__attribute__((used))
void kmain(uint64_t addr);

__attribute__((used))
void kmain(uint64_t addr) {
    kcls();
    kset_attr(PRINT_COLOR_YELLOW, PRINT_COLOR_BLACK);

    init_idt();
    init_timer();

    struct multiboot_tag *tag = (struct multiboot_tag *) (addr + 8);
    struct multiboot_tag_mmap *memory_map = NULL;

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
        }

        tag = (struct multiboot_tag *) ((uint8_t *) tag + ((tag->size + 7) & ~(uint32_t) 7));
    }

    if (memory_map == NULL) {
        panic("Could not find a memory map.");
    }
    init_paging(memory_map);
    if (!is_paging_initialized()) {
        panic("Paging was unable to successfully initialize.");
    }

    init_keyboard();

    for (;;);
}