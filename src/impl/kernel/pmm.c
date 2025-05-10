#include "pmm.h"
#include "multiboot.h"
#include "utility.h"
#include "paging.h"
#include "print.h"

#define BITS_PER_BYTE 8

static uint8_t *page_bitmap = NULL;
static uint64_t total_pages = 0;
static uint64_t highest_address = 0;
static uint64_t bitmap_size_bytes = 0;

static volatile uint64_t first_available_page_index = 0;

static uint64_t total_available_memory = 0;
static uint64_t used_page_count = 0;

extern char _kernel_start, _kernel_end, stack_top, stack_bottom;

static inline void bitmap_set(uint64_t page_index) {
    uint64_t byte_index = page_index / BITS_PER_BYTE;
    uint8_t bit_offset = (uint8_t) (page_index % BITS_PER_BYTE);
    if (byte_index < bitmap_size_bytes) {
        page_bitmap[byte_index] |= (uint8_t) (1U << bit_offset);
    }
}

static inline void bitmap_clear(uint64_t page_index) {
    uint64_t byte_index = page_index / BITS_PER_BYTE;
    uint8_t bit_offset = page_index % BITS_PER_BYTE;
    if (byte_index < bitmap_size_bytes) {
        page_bitmap[byte_index] &= (uint8_t) ~(1U << bit_offset);

        if (page_index < first_available_page_index) {
            first_available_page_index = page_index;
        }
    }
}

static inline bool bitmap_test(uint64_t page_index) {
    uint64_t byte_index = page_index / BITS_PER_BYTE;
    uint8_t bit_offset = page_index % BITS_PER_BYTE;
    if (byte_index < bitmap_size_bytes) {
        return (page_bitmap[byte_index] & (1U << bit_offset)) != 0;
    }
    return true;
}

static void pmm_mark_range_used(uint64_t start_page, uint64_t end_page) {
    for (uint64_t i = start_page; i < end_page; ++i) {
        if (i < total_pages) {
            if (!bitmap_test(i)) {
                used_page_count++;
            }
            bitmap_set(i);
        }
    }
}

void initialize_physical_allocator(struct multiboot_tag_mmap *mmap_tag) {
    kprintf("Initializing Physical Memory Allocator...\n");

    uint64_t kernel_end_page_aligned = ((uint64_t) &_kernel_end + PAGE_SIZE - 1) & ~((uint64_t) PAGE_SIZE - 1);

    highest_address = 0;

    for (multiboot_memory_map_t *mmap = mmap_tag->entries;
         (uint8_t *) mmap < (uint8_t *) mmap_tag + mmap_tag->size;
         mmap = (multiboot_memory_map_t *) ((uint8_t *) mmap + mmap_tag->entry_size)) {

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t region_end = mmap->addr + mmap->len;
            if (region_end > highest_address) {
                highest_address = region_end;
            }
        }
    }
    total_pages = highest_address / PAGE_SIZE;
    bitmap_size_bytes = (total_pages + 7) / 8;


    page_bitmap = NULL;
    uint64_t bitmap_phys_addr = 0;

    for (multiboot_memory_map_t *mmap = mmap_tag->entries;
         (uint8_t *) mmap < (uint8_t *) mmap_tag + mmap_tag->size;
         mmap = (multiboot_memory_map_t *) ((uint8_t *) mmap + mmap_tag->entry_size)) {

        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE && mmap->len >= bitmap_size_bytes) {
            uint64_t start_addr = mmap->addr;
            if (start_addr < kernel_end_page_aligned) {
                if (mmap->addr + mmap->len > kernel_end_page_aligned) {
                    start_addr = kernel_end_page_aligned;
                } else {
                    continue;
                }
            }

            uint64_t aligned_start = (start_addr + PAGE_SIZE - 1) & ~((uint64_t) PAGE_SIZE - 1);

            if (mmap->addr + mmap->len >= aligned_start + bitmap_size_bytes) {
                bitmap_phys_addr = aligned_start;
                page_bitmap = (uint8_t *) bitmap_phys_addr;

                break;
            }
        }
    }

    if (page_bitmap == NULL) {
        panic("Could not find suitable memory for page bitmap!");
    }
    memset(page_bitmap, 0x00, bitmap_size_bytes);
    total_available_memory = highest_address;
    used_page_count = 0;

    for (multiboot_memory_map_t *mmap = mmap_tag->entries;
         (uint8_t *) mmap < (uint8_t *) mmap_tag + mmap_tag->size;
         mmap = (multiboot_memory_map_t *) ((uint8_t *) mmap + mmap_tag->entry_size)) {

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) {
            uint64_t start_page = mmap->addr / PAGE_SIZE;

            uint64_t end_addr = mmap->addr + mmap->len;
            uint64_t end_page = (end_addr + PAGE_SIZE - 1) / PAGE_SIZE;

            for (uint64_t i = start_page; i < end_page; ++i) {
                if (i < total_pages) {
                    if (!bitmap_test(i)) {
                        bitmap_set(i);
                        used_page_count++;
                        total_available_memory -= PAGE_SIZE;
                    }
                }
            }
        }
    }

    pmm_mark_range_used(0, 0x100000 / PAGE_SIZE);
    pmm_mark_range_used((uint64_t) &_kernel_start / PAGE_SIZE, ((uint64_t) &_kernel_end + PAGE_SIZE - 1) / PAGE_SIZE);
    pmm_mark_range_used((uint64_t) &stack_bottom / PAGE_SIZE, ((uint64_t) &stack_top + PAGE_SIZE - 1) / PAGE_SIZE);
    pmm_mark_range_used(bitmap_phys_addr / PAGE_SIZE,
                        (bitmap_phys_addr + bitmap_size_bytes + PAGE_SIZE - 1) / PAGE_SIZE);

    first_available_page_index = 0;

    while (bitmap_test(first_available_page_index) && first_available_page_index < total_pages) {
        first_available_page_index++;
    }

    kprintf("Physical Allocator Initialized. First available page index hint: %llu\n", first_available_page_index);
}

uint64_t allocate_physical_page(void) {
    if (!page_bitmap) {
        kprintf("ERROR: Physical allocator not initialized!\n");
        return 0;
    }

    uint64_t limit_1gb_page = 0x40000000 / PAGE_SIZE;
    uint64_t search_limit = (limit_1gb_page < total_pages) ? limit_1gb_page : total_pages;
    if (is_paging_initialized()) {
        search_limit = total_pages;
    }

    for (uint64_t current_index = first_available_page_index; current_index < search_limit; ++current_index) {
        if (!bitmap_test(current_index)) {
            bitmap_set(current_index);

            first_available_page_index = current_index + 1;
            used_page_count++;

            uint64_t phys_addr = current_index * PAGE_SIZE;
            memset((void *) phys_addr, 0, PAGE_SIZE);
            return phys_addr;
        }
    }

    return 0;
}

void free_physical_page(uint64_t paddr) {
    if (!page_bitmap || paddr >= highest_address || (paddr % PAGE_SIZE != 0)) {
        kprintf("WARN: Attempt to free invalid physical address %p\n", (void *) paddr);
        return;
    }

    uint64_t page_index = paddr / PAGE_SIZE;

    used_page_count--;

    if (!bitmap_test(page_index)) {
        kprintf("WARN: Attempt to double-free physical page %llu (Addr: %p)\n", page_index, (void *) paddr);
        return;
    }

    bitmap_clear(page_index);
}


uint64_t get_total_memory(void) {
    return total_available_memory;
}

uint64_t get_used_memory(void) {
    return used_page_count * PAGE_SIZE;
}

uint64_t get_highest_addr(void) {
    return highest_address;
}