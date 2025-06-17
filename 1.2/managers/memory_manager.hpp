#ifndef MEMORY_MANAGER_HPP
#define MEMORY_MANAGER_HPP

// external libraries to be included
#include "../lib/stdint.hpp"
#include "../lib/multiboot.hpp"
// #include "../lib/printf.hpp"
#include "../lib/util.hpp"

#include "heap_manager.hpp"

// defines
extern uint32_t initial_page_directory[1024];
#define KERNEL_START 0xc0000000
#define PAGE_FLAG_PRESENT (1<<0)
#define PAGE_FLAG_WRITE (1<<1)
#define PAGE_FLAG_OWNER (1<<9)

// some more defines :zz
#define KERNEL_MALLOC 0xD000000
#define REC_PAGEDIR ((uint32_t*)0xFFFFF000)
#define REC_PAGETABLE(i) ((uint32_t*) (0xFFC00000 + ((i) << 12)))



class MemoryManager {
public:
    static void init(uint32_t memHigh, uint32_t physicalAllocationStart);

    static uint32_t alloc_page_frame();
    static void free_page_frame(uint32_t phys_addr);

    static void map_page(uint32_t vir_addr, uint32_t phys_addr, uint32_t flags);
    static uint32_t unmap_page(uint32_t vir_addr);

    static uint32_t *alloc_page_dir();
    static uint32_t * get_current_page_dir();
    static void change_page_dir(uint32_t * pd);
    static void sync_page_dirs();

private:
    static void invalidate(uint32_t virAddr);
    static void init_physical_memory(uint32_t memLow, uint32_t memHigh);


};


#endif