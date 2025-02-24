#include "kernel_memory.h"

static uint32_t heap_size;
static uint32_t heap_start;
static uint32_t limit;
static boolean kernel_memory_init = false;


void change_heap_size(int new_size){
    int old_page = CEIL_DIV(heap_size, 0x1000);
    int new_page = CEIL_DIV(new_size, 0x1000);

    if(new_page>old_page){
        int delta= new_page - old_page;
        for(int i =0; i< delta; i++){
            uint32_t physica_mem_location = physical_memory_alloc_page_frame();
            memory_map_page(KERNEL_MALLOC+old_page *0x1000+i*0x1000, physica_mem_location, PAGE_FLAG_WRITE);
        }
    }
    // printf("yassssssss\n");
    heap_size=new_size;
}



void init_kmalloc(uint32_t heap_size){
    heap_start = KERNEL_MALLOC;
    heap_size = 0;
    limit = 0;
    kernel_memory_init = true;

    change_heap_size(heap_size);
    *((uint32_t *) heap_start);
}


// Allocates memory from the kernel heap
void *kmalloc(uint32_t size) {
    
    if (!kernel_memory_init) {
        // If the kernel heap isn't initialized, return NULL or panic
        return NULL;
    }

    
    // Align size to the nearest multiple of 4 bytes (or 16 bytes for better alignment)
    size = (size + 3) & ~3;

    // Check if we have enough space in the heap
    if (heap_size + size > limit) {
        // If not, increase the heap size
        change_heap_size(heap_size + size);
    }
    
// printf("size %d\n", size);
    // Allocate memory at the current heap position
    uint32_t *allocated_mem = (uint32_t *)(heap_start + heap_size);

    // Update the heap size to reflect the allocation
    heap_size += size;

    // Return the allocated memory pointer
    return allocated_mem;
}




// Free a previously allocated kernel memory block
void kfree(void *ptr) {
    if (!ptr) {
        return; // Do nothing for NULL pointers
    }

    // Align the address to the page size (4 KB)
    uint32_t addr = (uint32_t)ptr;
    addr &= ~(0xFFF); // Clear lower 12 bits to get the page-aligned address

    // Unmap the page and free the associated physical frame
    uint32_t phys_addr = memory_unmap_page(addr); // Unmap and get physical address
    if (phys_addr) {
        physical_memory_free_page_frame(phys_addr); // Mark the physical frame as free
    }
}