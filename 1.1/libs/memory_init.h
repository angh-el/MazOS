#pragma once

#include "stdint.h"
#include "multiboot.h"
#include "printf.h"
#include "util.h"

extern uint32_t initial_page_directory[1024];
#define KERNEL_START 0xc0000000
#define PAGE_FLAG_PRESENT (1<<0)
#define PAGE_FLAG_WRITE (1<<1)

void init_memory(uint32_t memHigh, uint32_t physicalAllocationStart);
void invalidate(uint32_t virAddr);
void init_physical_memory(uint32_t memLow, uint32_t memHigh);