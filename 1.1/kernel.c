#include "drivers/screen.h"
#include "gdt.h"
#include "idt.h"
#include "libs/printf.h"
#include "drivers/keyboard.h"
#include "libs/multiboot.h"
#include "libs/memory_init.h"

#include "libs/kernel_memory.h"
#include "libs/process.h"
#include "drivers/disk_io.h"

#include "programs/calculator.h"
#include "programs/snake.h"



#include "timer.h"

void kmain(uint32_t magic, struct multiboot_info* bootInfo);
void print_logo();


void init_process_management();
void test_process_creation();


void test_disk_read() {
    if (disk_initialize() != 0) {
        printf("Disk initialization failed!\n");
        return;
    }


    uint32_t disk_size = get_disk_size();
    if (disk_size == -1) {
        print("Failed to retrieve disk size!\n");
        return;
    }
    printf("Disk size: ");
    printf("%d", disk_size); // Assuming print_int is a helper to display integers
    printf(" sectors\n");


    // uint8_t buffer[512];
    // if (read_sector(0, buffer) == 0) {
    //     print("Sector 0 read successfully:\n");
    //     for (int i = 0; i < 512; i++) {
    //         // print_char(buffer[i], i % 80, i / 80, 0x0F);
    //         printf("%c", buffer[i]);
    //     }
    // } else {
    //     printf("Failed to read sector 0\n");
    // }
}



void kmain(uint32_t magic, struct multiboot_info* bootInfo){
    clear_screen();
    print_logo(); 

    init_gdt();
    // print("Successfullly enabled the gdt ting\n");
    print("\n");
    
    init_idt();
    // print("Successfully enabled interrupts\n\n");


    init_keyboard();
    // print("Successfully enabled keyboard driver\n");

    uint32_t mod = *(uint32_t*)(bootInfo->mods_addr+4);
    uint32_t physicalAllocationStart = (mod + 0xfff) & ~0xfff;
    
    printf("bootInfo->mem_upper: %d KB\n", bootInfo->mem_upper);
    printf("bootInfo->mods_addr: %p\n", bootInfo->mods_addr);
    printf("physicalAllocationStart: %p\n", physicalAllocationStart);

    init_memory(bootInfo->mem_upper * 1024, physicalAllocationStart);
    // print("Successfully Allocated Memory\nPaging Done?\n");
    

    memory_map_page((uint32_t)bootInfo, (uint32_t)bootInfo, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);


    // print("Allocating memory for kernel\n");
    init_kmalloc(0x1000);
    
    // calculator();
    // start_snake_game();


    test_disk_read();


    // Initialize the process management system
    // init_process_management();

    // Test process creation and context switching
    // test_process_creation();

    // clear_screen();

    // init_timer();

    for(;;);
}


void print_logo(){
    print("\n\n\n\n\n\n\n\n");
    print("            ##     ##    ###    ########     #######   ######  \n");
    print("            ###   ###   ## ##        ##     ##     ## ##    ## \n");
    print("            #### ####  ##   ##      ##      ##     ## ##       \n");
    print("            ## ### ## ##     ##    ##       ##     ##  ######  \n");
    print("            ##     ## #########   ##        ##     ##       ##\n");
    print("            ##     ## ##     ##  ##         ##     ## ##    ## \n");
    print("            ##     ## ##     ## ########     #######   ######  \n");
    print("\n\n\n\n\n\n\n\n");
}


// Function to initialize process management
void init_process_management() {
    // Initialize memory for PCB and other process management structures.
    print("Initializing process management...\n");
    // Your implementation may go here to initialize process-related structures, etc.
    init_processes();
}



// Example process function 1
void process1_function() {
    print("Running process 1\n");
    for (int i = 0; i < 5; i++) {
        print("Process 1 - Count: ");
        printf("%d",i);
        print("\n");
    }
    print("Process 1 finished!\n");
}

// Example process function 2
void process2_function() {
    print("Running process 2\n");
    for (int i = 0; i < 5; i++) {
        print("Process 2 - Count: ");
        printf("%d",i);
        print("\n");
    }
    print("Process 2 finished!\n");
}

// Function to test process creation
void test_process_creation() {
    print("Creating test processes...\n");

    // Allocate memory for PCB (this is where the memory allocation should happen)
    PCB *process1 = (PCB*) kmalloc(sizeof(PCB));  // Allocate memory for the first PCB
    PCB *process2 = (PCB*) kmalloc(sizeof(PCB));  // Allocate memory for the second PCB

    // Now that the PCBs are allocated, we can safely map their memory pages
    memory_map_page((uint32_t)process1, (uint32_t)process1, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    // memory_map_page((uint32_t)process1->stack, (uint32_t)process1->stack, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    memory_map_page((uint32_t)process2, (uint32_t)process2, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

    // // Create the test processes and initialize their PCBs
    process1 = create_process((uint32_t)process1_function); // Store the PCB pointer for process1
    
    process2 = create_process((uint32_t)process2_function); // Store the PCB pointer for process2
    

    // printf("stack: %p\n", &process1->stack);
    // printf("p1_function %p\n", ((uint32_t)process1_function));
    // print("Testing context switch...\n");
    
    
    // Perform a context switch between the two processes
    // context_switch(process1, process2);
}
