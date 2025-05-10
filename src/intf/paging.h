//
// Created by RHM on 01/05/2025.
//

#ifndef RHOS_PAGING_H
#define RHOS_PAGING_H

#include "kstdlib.h"
#include "multiboot.h"

#define KERNEL_VIRT_BASE 0xFFFF800000000000

#define PTE_PRESENT   (1ULL << 0)
#define PTE_WRITABLE  (1ULL << 1)
#define PTE_USER      (1ULL << 2)
#define PTE_PWT       (1ULL << 3) // Page Write Through
#define PTE_PCD       (1ULL << 4) // Page Cache Disable
#define PTE_ACCESSED  (1ULL << 5)
#define PTE_DIRTY     (1ULL << 6)
#define PTE_HUGE_PAGE (1ULL << 7) // For 2MB or 1GB pages
#define PTE_GLOBAL    (1ULL << 8) // Global page (not invalidated on CR3 load)
#define PTE_NX        (1ULL << 63) // No Execute

void init_paging(struct multiboot_tag_mmap *memory_map);

bool is_paging_initialized(void);

#endif //RHOS_PAGING_H
