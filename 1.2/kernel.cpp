// libs
#include "lib/multiboot.hpp"
#include "lib/stdint.hpp"
#include "lib/util.hpp"

// descriptors (IDT & GDT)
#include "descriptors/gdt.hpp"
#include "descriptors/idt.hpp"

// managers
#include "managers/memory_manager.hpp"      // memory manager
#include "managers/heap_manager.hpp"        // heap manager
#include "managers/process_manager.hpp"     // process manager


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



    // TODO: Syscalls



    // TODO CLI

    return 0;
}