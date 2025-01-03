#include "gdt.h"

extern void gdt_flush(addr_t);

struct gdt_entry gdt_entries[5];
struct gdt gdt_ptr;

void init_gdt(){
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 5) -1 ;
    gdt_ptr.base = &gdt_entries;

    set_gdt(0, 0, 0, 0, 0);                 // null segment
    set_gdt(1, 0, 0xffffffff, 0x9a, 0xcf);  // kernel code segment
    set_gdt(2, 0, 0xffffffff, 0x92, 0xcf);  // kernel data segment
    set_gdt(3, 0, 0xffffffff, 0xfa, 0xcf);  // user code segment
    set_gdt(4, 0, 0xffffffff, 0xf2, 0xcf);  // user data segment

    gdt_flush(&gdt_ptr);
}

void set_gdt(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint16_t gran){
    gdt_entries[num].base_low = (base & 0xffff);
    gdt_entries[num].base_mid = (base >> 16) & 0xff;
    gdt_entries[num].base_high = (base >> 24) & 0xff;
    gdt_entries[num].limit = (limit & 0xffff);
    gdt_entries[num].flags = (limit >> 16) & 0x0f;
    gdt_entries[num].flags |= (gran & 0xf0);
    gdt_entries[num].access = access;
}