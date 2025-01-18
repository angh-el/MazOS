#include "drivers/screen.h"
#include "gdt.h"
#include "idt.h"
#include "libs/printf.h"
#include "drivers/keyboard.h"
#include "libs/multiboot.h"
#include "libs/memory_init.h"

#include "libs/kernel_memory.h"

#include "timer.h"

void kmain(uint32_t magic, struct multiboot_info* bootInfo);
void print_logo();


void kmain(uint32_t magic, struct multiboot_info* bootInfo){
    clear_screen();
    print_logo(); 

    init_gdt();
    print("Successfullly enabled the gdt ting\n");
    print("\n");
    
    init_idt();
    print("Successfully enabled interrupts\n\n");


    init_keyboard();
    print("Successfully enabled keyboard driver\n");

    uint32_t mod = *(uint32_t*)(bootInfo->mods_addr+4);
    uint32_t physicalAllocationStart = (mod + 0xfff) & ~0xfff;
    
    init_memory(bootInfo->mem_upper * 1024, physicalAllocationStart);
    print("Successfully Allocated Memory\nPaging Done?\n");
    

    memory_map_page((uint32_t)bootInfo, (uint32_t)bootInfo, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);


    print("Allocating memory for kernel\n");
    init_kmalloc(0x1000);
    // print("kmalloc successfuly init ??\n");


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