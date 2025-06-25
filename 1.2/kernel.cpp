// libs
#include "lib/multiboot.hpp"
#include "lib/stdint.hpp"
#include "lib/util.hpp"
#include "lib/printf.hpp"

// descriptors (IDT & GDT)
#include "descriptors/gdt.hpp"
#include "descriptors/idt.hpp"

// managers
#include "managers/memory_manager.hpp"      // memory manager
#include "managers/heap_manager.hpp"        // heap manager
#include "managers/process_manager.hpp"     // process manager

// dirvers                                  
#include "drivers/display/display.hpp"      // display driver
#include "drivers/display/graphics.hpp"     // graphics driver
#include "drivers/input/keyboard.hpp"       // keyboard driver
#include "drivers/input/mouse.hpp"          // mouse driver
#include "drivers/storage/disk.hpp"         // disk driver
#include "drivers/storage/fat32.hpp"        // fat32 driver
#include "drivers/timer.hpp"                // timer driver
#include "drivers/sound.hpp"                // sound driver

// syscall
#include "syscall/syscall.hpp"              // syscall handler

// command line interface
#include "user_space/cli.hpp"               // command line interface

// networking
#include "networking/pci.hpp"               // pci
#include "networking/rtl8139.hpp"           // rtl8139 driver

int main(uint32_t magic, struct multiboot_info* bootInfo){

    // initialise the gdt and idt tables
    DescriptorTables::GDT::init();
    DescriptorTables::IDT::init();
    

    // initialse memory
    uint32_t mod = *(uint32_t*)(bootInfo->mods_addr+4);
    uint32_t physicalAllocationStart = (mod + 0xfff) & ~0xfff;
    MemoryManager::init(bootInfo->mem_upper * 1024, physicalAllocationStart);
    MemoryManager::map_page((uint32_t)bootInfo, (uint32_t)bootInfo, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    // allocate memory for the kernel on the heap
    init_kmalloc(0x1000);


    // process manager
    ProcessManager processManager;
    processManager.init();


    // TODO: Drivers
    Display::print("Display works\bs\n");
    
    // // graphics mode tests
    // Graphics::init();
    // Graphics::clear_screen(0);
    // Graphics::draw_diagonal_line();

    // keyboard driver
    Keyboard keyboard;
    keyboard.init();

    // mouse driver
    Mouse mouse;
    mouse.init();

    // disk driver
    Disk::init();
    printf("Disk size: %d\n", Disk::get_size());

    // Fat32 driver
    Fat32 fat32;
    fat32.mount(0);
    fat32.ls();
    
    // timer
    Timer::init();
    Timer::terminate();

    // speaker
    Sound::init();


    // TODO: Syscalls
    Syscall::init();


    // TODO CLI
    // CLI::init(fat32);


    // networking
    // printf("%d", rtl8139_init());
    // if (rtl8139_init()) {
    //     printf("RTL8139 initialized successfully!\n");
    //     print_mac();
    //     // uint8_t mac[6];
    //     // rtl8139_get_mac(mac);
    //     // printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //     //        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    // } 
    // else {
    //     printf("RTL8139 initialization failed.\n");
    // }

    initialize_rtl8139_with_explicit_params();

    for(;;)

    return 0;
}





