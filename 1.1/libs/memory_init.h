#pragma once

#include "stdint.h"
#include "multiboot.h"
#include "printf.h"
#include "util.h"

extern uint32_t initial_page_directory[1024];
#define KERNEL_START 0xc0000000
#define PAGE_FLAG_PRESENT (1<<0)
#define PAGE_FLAG_WRITE (1<<1)
#define PAGE_FLAG_OWNER (1<<9)

void init_memory(uint32_t memHigh, uint32_t physicalAllocationStart);
void invalidate(uint32_t virAddr);
void init_physical_memory(uint32_t memLow, uint32_t memHigh);

// kernel memory allocation code:
uint32_t * memory_get_current_page_dir();
uint32_t physical_memory_alloc_page_frame();
void memory_change_page_dir(uint32_t * pd);
void memory_map_page(uint32_t vir_addr, uint32_t phys_addr, uint32_t flags);
void sync_page_dirs();

uint32_t *memory_alloc_page_dir();
uint32_t memory_unmap_page(uint32_t vir_addr);
void physical_memory_free_page_frame(uint32_t phys_addr);

#define KERNEL_MALLOC 0xD000000
#define REC_PAGEDIR ((uint32_t*)0xFFFFF000)
#define REC_PAGETABLE(i) ((uint32_t*) (0xFFC00000 + ((i) << 12)))
