#pragma once

#include "stdint.h"
#include "util.h"
#include "memory_init.h"

void init_kmalloc(uint32_t heap_size);

void change_heap_size(int new_size);

void *kmalloc(uint32_t size);

void kfree(void *ptr);