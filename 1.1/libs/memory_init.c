#include "memory_init.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t totalAlloc;

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000/0x100/8)

uint8_t physical_memory_bitmap[NUM_PAGE_FRAMES/8];
static uint32_t page_dirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t page_dirs_used[NUM_PAGE_DIRS];


void init_memory(uint32_t memHigh, uint32_t physicalAllocationStart){
    initial_page_directory[0] = 0;
    invalidate(0);

    initial_page_directory[1023] = ((uint32_t)initial_page_directory-KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate(0xfffff000);

    init_physical_memory(physicalAllocationStart, memHigh);
    memset(page_dirs, 0, 0x1000*NUM_PAGE_DIRS);
    memset(page_dirs_used, 0, NUM_PAGE_DIRS);
}

void invalidate(uint32_t virAddr){
    asm volatile("invlpg %0" :: "m"(virAddr));
}


void init_physical_memory(uint32_t memLow, uint32_t memHigh){
    page_frame_min = CEIL_DIV(memLow, 0x1000);
    page_frame_max = memHigh/0x1000;
    totalAlloc = 0;
    memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}