#include "multiboot.h"
#include "paging.h"
#include "utility.h"
#include "pmm.h"
#include "print.h"

extern char _kernel_start, _kernel_end;

typedef uint64_t page_table_entry_t;
typedef page_table_entry_t page_table[512];

page_table *new_pml4_virt_addr = NULL;

void map_page(page_table *pml4, uint64_t virtual_address, uint64_t physical_address, uint64_t flags);

void map_range(page_table *pml4, uint64_t virt_start, uint64_t phys_start, uint64_t size, uint64_t flags);

void init_paging(struct multiboot_tag_mmap *mmap_tag) {
    kprintf("Initializing new page tables...\n");

    initialize_physical_allocator(mmap_tag);
    kprintf("RAM Available: %lluKiB (%lluMiB); RAM Used: %lluKiB (%lluMiB)\n",
            get_total_memory() / 1024, get_total_memory() / 1024 / 1024,
            get_used_memory() / 1024, get_used_memory() / 1024 / 1024);

    uint64_t new_pml4_phys_addr = allocate_physical_page();
    if (new_pml4_phys_addr == 0) {
        panic("Failed to allocate memory for new PML4!");
    }

    new_pml4_virt_addr = (page_table *) new_pml4_phys_addr;

    map_range(new_pml4_virt_addr, (uint64_t) &_kernel_start,
              (uint64_t) &_kernel_start, (uint64_t) &_kernel_end - (uint64_t) &_kernel_start,
              PTE_PRESENT | PTE_WRITABLE);


    map_range(new_pml4_virt_addr, KERNEL_VIRT_BASE,
              (uint64_t) &_kernel_start, (uint64_t) &_kernel_end - (uint64_t) &_kernel_start,
              PTE_PRESENT | PTE_WRITABLE);

    map_page(new_pml4_virt_addr, (uint64_t) new_pml4_virt_addr, new_pml4_phys_addr, PTE_PRESENT | PTE_WRITABLE);

    for (multiboot_memory_map_t *mmap = mmap_tag->entries;
         (uint8_t *) mmap < (uint8_t *) mmap_tag + mmap_tag->size;
         mmap = (multiboot_memory_map_t *) ((uint8_t *) mmap + mmap_tag->entry_size)) {

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE) continue;

        uint64_t phys_start = mmap->addr;
        uint64_t len = mmap->len;
        uint64_t virt_start = phys_start;

        kprintf("Mapping available memory (phys: %p, len: %llu) to virt: %p...\n",
                (void *) phys_start, len, (void *) virt_start);

        while (len >= 0x200000 && (phys_start % 0x200000) == 0 && (virt_start % 0x200000) == 0) {
            map_page(new_pml4_virt_addr, virt_start, phys_start, PTE_PRESENT | PTE_WRITABLE | PTE_HUGE_PAGE);
            phys_start += 0x200000;
            virt_start += 0x200000;
            len -= 0x200000;
        }

        while (len > 0) {
            map_page(new_pml4_virt_addr, virt_start, phys_start, PTE_PRESENT | PTE_WRITABLE);
            phys_start += PAGE_SIZE;
            virt_start += PAGE_SIZE;
            len -= PAGE_SIZE;
        }
    }

    map_page(new_pml4_virt_addr, 0xB8000,
             0xB8000, PTE_PRESENT | PTE_WRITABLE | PTE_PCD |
                      PTE_PWT | PTE_NX);

    kprintf("Switching to new page tables (CR3 = %p)...\n", (void *) new_pml4_phys_addr);
    __asm__ volatile("mov %0, %%cr3"::"r"(new_pml4_phys_addr) : "memory");
}

void map_page(page_table *pml4, uint64_t virtual_address, uint64_t physical_address, uint64_t flags) {
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
    uint64_t pdpt_index = (virtual_address >> 30) & 0x1FF;
    uint64_t pd_index = (virtual_address >> 21) & 0x1FF;
    uint64_t pt_index = (virtual_address >> 12) & 0x1FF;

    page_table_entry_t *pml4_entry = &(*pml4)[pml4_index];

    if (!(*pml4_entry & PTE_PRESENT)) {
        uint64_t new_pdpt_phys = allocate_physical_page();
        if (new_pdpt_phys == 0) {
            panic("map_page: Failed to allocate physical page for PDPT");
        }
        memset((void *) new_pdpt_phys, 0, PAGE_SIZE);

        *pml4_entry = new_pdpt_phys | PTE_PRESENT | PTE_WRITABLE;
    }

    page_table *pdpt = (page_table *) (*pml4_entry & ~0xFFFULL);
    page_table_entry_t *pdpt_entry = &(*pdpt)[pdpt_index];

    if ((flags & PTE_HUGE_PAGE) && ((virtual_address % 0x40000000) == 0) && ((physical_address % 0x40000000) == 0)) {
        if ((*pdpt_entry & PTE_PRESENT) && !(*pdpt_entry & PTE_HUGE_PAGE)) {
            panic("map_page: Attempting to map 1GB huge page over existing Page Directory table!");
        }
        *pdpt_entry = (physical_address & ~0x3FFFFFULL) | flags | PTE_HUGE_PAGE;

        __asm__ volatile ("invlpg (%0)"::"r"(virtual_address) : "memory");
        return;
    }

    if (!(*pdpt_entry & PTE_PRESENT)) {
        uint64_t new_pd_phys = allocate_physical_page();
        if (new_pd_phys == 0) {
            panic("map_page: Failed to allocate physical page for PD");
        }
        memset((void *) new_pd_phys, 0, PAGE_SIZE);

        *pdpt_entry = new_pd_phys | PTE_PRESENT | PTE_WRITABLE;
    } else if (*pdpt_entry & PTE_HUGE_PAGE) {
        panic("map_page: Attempting to map 4KB/2MB page over existing 1GB huge page!");
    }

    page_table *pd = (page_table *) (*pdpt_entry & ~0xFFFULL);
    page_table_entry_t *pd_entry = &(*pd)[pd_index];

    if ((flags & PTE_HUGE_PAGE) && ((virtual_address % 0x200000) == 0) && ((physical_address % 0x200000) == 0)) {
        if ((*pd_entry & PTE_PRESENT) && !(*pd_entry & PTE_HUGE_PAGE)) {
            panic("map_page: Attempting to map 2MB huge page over existing Page Table!");
        }
        *pd_entry = (physical_address & ~0x1FFFFFULL) | flags | PTE_HUGE_PAGE;

        __asm__ volatile ("invlpg (%0)"::"r"(virtual_address) : "memory");
        return;
    }

    if (!(*pd_entry & PTE_PRESENT)) {
        uint64_t new_pt_phys = allocate_physical_page();
        if (new_pt_phys == 0) {
            panic("map_page: Failed to allocate physical page for PT");
        }
        memset((void *) new_pt_phys, 0, PAGE_SIZE);

        *pd_entry = new_pt_phys | PTE_PRESENT | PTE_WRITABLE;
    } else if (*pd_entry & PTE_HUGE_PAGE) {
        panic("map_page: Attempting to map 4KB page over existing 2MB huge page!");
    }

    page_table *pt = (page_table *) (*pd_entry & ~0xFFFULL);
    page_table_entry_t *page_entry = &(*pt)[pt_index];

    *page_entry = (physical_address & ~0xFFFULL) | (flags & ~PTE_HUGE_PAGE);

    __asm__ volatile ("invlpg (%0)"::"r"(virtual_address) : "memory");
}

void map_range(page_table *pml4, uint64_t virt_start, uint64_t phys_start, uint64_t size, uint64_t flags) {
    for (uint64_t offset = 0; offset < size; offset += PAGE_SIZE) {
        map_page(pml4, virt_start + offset, phys_start + offset, flags);
    }
}

bool is_paging_initialized(void) {
    return new_pml4_virt_addr != NULL;
}