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

