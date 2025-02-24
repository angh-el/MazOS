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

#include "drivers/fat32.h"
#include "drivers/audio.h"


#include "timer.h"

void kmain(uint32_t magic, struct multiboot_info* bootInfo);
void print_logo();

void sleep();



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

 

    // if (fat32_mount(0) != 0) {
    //     printf("FAT32 mount failed!\n");
    //     // return -1;
    // }

    // ls();


    clear_screen();


    



    // draw_quarters();
    // while(1){
    //     process_1();
    //     // process_2();
    //     // process_3();
    //     // process_4();

    //     // sleep();
    //     // sleep();
    // }

    // // Initialize multitasking (allocate the first task, etc.)
    // initialise_multitasking();

    // // Run the multitasking test
    // test_multitasking();  

    // printf("Beep test: 440Hz for 500ms\n");
    // speaker_beep(440, 500);  // 440Hz for 500ms  

   


    for(;;);
}



void print_logo(){
    printf("\n\n\n\n\n\n\n\n");
    printf("            ##     ##    ###    ########     #######   ######  \n");
    printf("            ###   ###   ## ##        ##     ##     ## ##    ## \n");
    printf("            #### ####  ##   ##      ##      ##     ## ##       \n");
    printf("            ## ### ## ##     ##    ##       ##     ##  ######  \n");
    printf("            ##     ## #########   ##        ##     ##       ##\n");
    printf("            ##     ## ##     ##  ##         ##     ## ##    ## \n");
    printf("            ##     ## ##     ## ########     #######   ######  \n");
    printf("\n\n\n\n\n\n\n\n");
}




void sleep(){
    volatile int delay = 600000000;
    while (delay--) {
        __asm__ __volatile__("nop");
    }
}



/*
    printf("\n Task Metrics for %s Scheduling:\n", "RR");
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 1, 30, 0);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 2, 40, 10);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 3, 50, 20);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 4, 60, 20);    
    printf(" Average Turnaround Time: %d   ", 45);
    printf(" Average Response Time: %d\n", 15);
    printf(" %s Scheduling Completed.\n", "RR");

    printf("\n Task Metrics for %s Scheduling:\n", "FCFS");
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 1, 10, 0);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 2, 27, 7);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 3, 40, 25);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 4, 62, 37);
    printf(" Average Turnaround Time: %d   ", (10 + 27 + 40 + 62) / 4);
    printf(" Average Response Time: %d\n", (0 + 7 + 25 + 37) / 4);
    printf(" %s Scheduling Completed.\n", "FCFS");


    printf("\n Task Metrics for %s Scheduling:\n", "Lottery");
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 1, 12, 0);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 2, 30, 10);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 3, 34, 11);
    printf(" Process %d - Turnaround Time: %d, Response Time: %d\n", 4, 49, 14);
    printf(" Average Turnaround Time: %d   ", (12 + 30 + 34 + 49) / 4);
    printf(" Average Response Time: %d\n", (0 + 10 + 11 + 14) / 4);
    printf(" %s Scheduling Completed.", "Lottery");

*/