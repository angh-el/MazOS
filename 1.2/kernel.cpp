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
    CLI::init(fat32);


    for(;;)

    return 0;
}





