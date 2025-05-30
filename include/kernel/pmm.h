//
// Created by RHM on 02/05/2025.
//

#ifndef RHOS_PMM_H
#define RHOS_PMM_H

#include "multiboot.h"

#define PAGE_SIZE 4096

void initialize_physical_allocator(struct multiboot_tag_mmap *mmap_tag);

uint64_t allocate_physical_page(void);

void free_physical_page(uint64_t paddr);


uint64_t get_total_memory(void);

uint64_t get_used_memory(void);

uint64_t get_highest_addr(void);

#endif //RHOS_PMM_H
