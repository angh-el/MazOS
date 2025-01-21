#include "memory_init.h"

static uint32_t page_frame_min;
static uint32_t page_frame_max;
static uint32_t totalAlloc;
int memory_num_vir_pages;

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000/0x1000/8)

uint8_t physical_memory_bitmap[NUM_PAGE_FRAMES/8];
static uint32_t page_dirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static uint8_t page_dirs_used[NUM_PAGE_DIRS];


void init_memory(uint32_t memHigh, uint32_t physicalAllocationStart){
    initial_page_directory[0] = 0;
    invalidate(0);
    // printf("aaaaamm\n");
    initial_page_directory[1023] = ((uint32_t)initial_page_directory-KERNEL_START) | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate(0xfffff000);
    
    init_physical_memory(physicalAllocationStart, memHigh);
    memset(page_dirs, 0, 0x1000*NUM_PAGE_DIRS);
    memset(page_dirs_used, 0, NUM_PAGE_DIRS);
}

void invalidate(uint32_t virAddr){
    asm volatile("invlpg %0" :: "m"(virAddr));
    // asm volatile("invlpg (%0)" ::"r" (virAddr) : "memory");
}


void init_physical_memory(uint32_t memLow, uint32_t memHigh){
    page_frame_min = CEIL_DIV(memLow, 0x1000);
    page_frame_max = memHigh/ 0x1000;
    
    printf("page_frame_min: %x\n", page_frame_min);
    printf("page_frame_max: %x\n", page_frame_max);

    totalAlloc = 0;
    memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}






uint32_t physical_memory_alloc_page_frame(){
    uint32_t start = page_frame_min/8 + ((page_frame_min &7)!= 0 ? 1 : 0);
    uint32_t end = page_frame_max/8 - ((page_frame_max &7)!= 0 ? 1 : 0);
    
    printf("start: %d\n", start);
    printf("end: %d\n", end);
    // for(uint32_t i  = start; i < end; i++){
    //     uint8_t byte = physical_memory_bitmap[i];
    //     if(byte == 0xff) continue;

    //     for(uint32_t j = 0; j < 8; j++){
    //         boolean taken = byte >> j&1;

    //         if(!taken){
    //             byte ^= (-1 ^ byte) & (1 << i);
    //             physical_memory_bitmap[i] = byte;
    //             totalAlloc++;
    //             uint32_t addy = (j*8*i) * 0x1000;
    //             return addy;
    //         }
    //     }
    // }

    for (uint32_t b = start; b < end; b++){
        uint8_t byte = physical_memory_bitmap[b];
        printf("YOOOOOOOOOOO: %d ",byte);
        if (byte == 0xFF){
            continue;
        }

        for (uint32_t i = 0; i < 8; i++){
            boolean used = byte >> i & 1;

            if (!used){
                byte ^= (-1 ^ byte) & (1 << i);
                physical_memory_bitmap[b] = byte;
                totalAlloc++;

                uint32_t addr = (b*8*i) * 0x1000;
                return addr;
            }
        }
        
    }


    return 0;
}


void memory_map_page(uint32_t vir_addr, uint32_t phys_addr, uint32_t flags){
    uint32_t* previous_page_dir = 0;
    
    if(previous_page_dir >= KERNEL_START){
        previous_page_dir = memory_get_current_page_dir();
        if(previous_page_dir != initial_page_directory) memory_change_page_dir(initial_page_directory);
    }

    uint32_t page_dir_idx = vir_addr>>22;
    uint32_t ptr_idx = vir_addr>>12 & 0x3ff;
    uint32_t *page_dir = REC_PAGEDIR;
    uint32_t *ptr = REC_PAGETABLE(page_dir_idx);

    if(!(page_dir[page_dir_idx] & PAGE_FLAG_PRESENT)){
        uint32_t ptr_page_addr = physical_memory_alloc_page_frame();
        // printf("AHHHHHHHHHHHHHHHHH: %d\n\n", ptr_page_addr);
        page_dir[page_dir_idx] = ptr_page_addr | PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE | PAGE_FLAG_OWNER | flags;

        invalidate(vir_addr);

        for(uint32_t i= 0; i < 1024; i++){ptr[i]=0;}
    }

    ptr[ptr_idx] = phys_addr | PAGE_FLAG_PRESENT | flags;
    memory_num_vir_pages ++;
    invalidate(vir_addr);

    if(previous_page_dir!=0){
        sync_page_dirs();

        if(previous_page_dir != initial_page_directory){
            memory_change_page_dir(previous_page_dir);
        }
    }
}


uint32_t * memory_get_current_page_dir(){
    uint32_t page_dir;
    asm volatile("mov %%cr3, %0": "=r"(page_dir));

    page_dir +=KERNEL_START;
    return (uint32_t *)
    page_dir;
}


void memory_change_page_dir(uint32_t * pd){
    pd = (uint32_t*) (((uint32_t)pd)-KERNEL_START);
    asm volatile("mov %0, %%eax \n mov %%eax, %%cr3 \n" :: "m"(pd));
    
}


void sync_page_dirs(){
    for(int i = 0; i<NUM_PAGE_DIRS; i++){
        if(page_dirs_used[i]){
            uint32_t *page_dir = page_dirs[i];
            for(int j = 768; j< 1023; j++){
                page_dir[j] = initial_page_directory[j] & ~PAGE_FLAG_OWNER;
            }
        }
    }
}





// uint32_t *memory_alloc_page_dir() {
//     // Allocate a page frame for the new page directory
//     uint32_t page_frame_addr = physical_memory_alloc_page_frame();
//     if (!page_frame_addr) {
//         return NULL; // Return NULL if no page frame is available
//     }

//     // Map the page frame into virtual memory space
//     uint32_t *new_page_dir = (uint32_t *)(page_frame_addr + KERNEL_START);

//     // Initialize the page directory
//     memset(new_page_dir, 0, 1024 * sizeof(uint32_t));

//     // Copy kernel mappings (indices 768 to 1023) from the initial page directory
//     for (int i = 768; i < 1024; i++) {
//         new_page_dir[i] = initial_page_directory[i];
//     }

//     // Return a pointer to the new page directory
//     return new_page_dir;
// }




uint32_t *memory_alloc_page_dir() {
    // Allocate a page frame for the new page directory
    uint32_t page_frame_addr = physical_memory_alloc_page_frame();
    printf("Page Frame: %d\n", page_frame_addr);
    if (!page_frame_addr) {
        printf("Error: Failed to allocate physical memory for page directory\n");
        return NULL; // Return NULL if no page frame is available
    }

    // Debug: print the physical address allocated
    printf("Allocated physical memory for page directory at: %p\n", (void*) page_frame_addr);

    // Map the page frame into virtual memory space
    // Ensure that KERNEL_START is correctly defined. Assuming KERNEL_START is the base of the kernel virtual address space
    uint32_t *new_page_dir = (uint32_t *)(page_frame_addr + KERNEL_START);

    // Debug: print the virtual address of the new page directory
    printf("Mapped page directory to virtual address: %p\n", (void*) new_page_dir);

    // Initialize the page directory (clear it out)
    memset(new_page_dir, 0, 1024 * sizeof(uint32_t));

    // Debug: Check if the new page directory was correctly cleared
    printf("Page directory initialized to zero\n");

    // Copy kernel mappings (indices 768 to 1023) from the initial page directory
    for (int i = 768; i < 1024; i++) {
        new_page_dir[i] = initial_page_directory[i];
    }

    // Debug: Check if the kernel mappings were successfully copied
    printf("Kernel page mappings copied to new page directory\n");

    // Return a pointer to the new page directory
    return new_page_dir;
}





// Unmap a virtual page and return its physical address
uint32_t memory_unmap_page(uint32_t vir_addr) {
    uint32_t page_dir_idx = vir_addr >> 22;
    uint32_t page_table_idx = (vir_addr >> 12) & 0x3FF;

    uint32_t *page_dir = REC_PAGEDIR;
    uint32_t *page_table = REC_PAGETABLE(page_dir_idx);

    if (!(page_dir[page_dir_idx] & PAGE_FLAG_PRESENT)) {
        return 0; // Page directory entry not present
    }

    uint32_t phys_addr = page_table[page_table_idx] & ~0xFFF; // Get physical address
    page_table[page_table_idx] = 0; // Clear the page table entry

    invalidate(vir_addr); // Invalidate the TLB for the unmapped page

    return phys_addr;
}


// Mark a physical page frame as free
void physical_memory_free_page_frame(uint32_t phys_addr) {
    uint32_t frame_number = phys_addr / 0x1000; // Convert address to frame number
    uint32_t byte_idx = frame_number / 8;      // Find the byte in the bitmap
    uint32_t bit_idx = frame_number % 8;       // Find the bit in the byte

    physical_memory_bitmap[byte_idx] &= ~(1 << bit_idx); // Mark the frame as free
    totalAlloc--;
}
