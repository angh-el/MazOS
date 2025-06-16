// libs
#include "lib/multiboot.hpp"
#include "lib/stdint.hpp"
#include "lib/util.hpp"

// descriptors (IDT & GDT)
#include "descriptors/gdt.hpp"
#include "descriptors/idt.hpp"



int main(uint32_t magic, struct multiboot_info* bootInfo){

    DescriptorTables::GDT::init();
    DescriptorTables::IDT::init();


    return 0;
}