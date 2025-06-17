#ifndef HEAP_MANAGER_HPP
#define HEAP_MANAGER_HPP

// library includes
#include "../lib/stdint.hpp"
#include "../lib/util.hpp"
#include "memory_manager.hpp"


// typedef struct {
//     static uint32_t heap_size;
//     static uint32_t heap_start;
//     static uint32_t limit;
//     static boolean kernel_memory_init;
// } KernelHeapManager;


void init_kmalloc(uint32_t heap_size);
void change_heap_size(int new_size);
void *kmalloc(uint32_t size);
void kfree(void *ptr);


#endif