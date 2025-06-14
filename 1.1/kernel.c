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
#include "programs/paint.h"
#include "programs/cli.h"

#include "drivers/fat32.h"
#include "drivers/audio.h"
#include "drivers/graphics.h"
#include "drivers/mouse.h"


#include "timer.h"

void kmain(uint32_t magic, struct multiboot_info* bootInfo);
void print_logo();

void sleep();

void test(){
    printf("HELLLOOO");
    sleep();
    sleep();
    // start_snake_game();
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

    // init_kmalloc(0x100000);
    
    // calculator();
    // test();
    // start_snake_game();


    if (fat32_mount(0) != 0) {
        printf("FAT32 mount failed!\n");
        // return -1;
    }

    init_syscalls();

    ls();

    cd("IMAGES     ");
    ls();
    cd_up("..         ");

    clear_screen();
    // init_mouse();

    // paint();
    // start_snake_game();

    init_cli();
    // clear_screen();


    // test_memory_management();

    // clear_screen();
    // paint();
    // sleep();

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

