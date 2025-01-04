#include "gdt.h"
#include "util.h"

extern void gdt_flush(uint32_t);
extern void tss_flush();

struct gdt_entry gdt_entries[6];
struct gdt gdt_ptr;
struct tss_entry_struct tss_entry;

void init_gdt(){
    gdt_ptr.limit = (sizeof(struct gdt_entry) * 6) -1 ;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    set_gdt(0, 0, 0, 0, 0);                 // null segment
    set_gdt(1, 0, 0xffffffff, 0x9a, 0xcf);  // kernel code segment
    set_gdt(2, 0, 0xffffffff, 0x92, 0xcf);  // kernel data segment
    // set_gdt(3, 0, 0xffffffff, 0xfa, 0xcf);  // user code segment
    // set_gdt(4, 0, 0xffffffff, 0xf2, 0xcf);  // user data segment
    set_tss(5, 0x10, 0x00);

    gdt_flush((uint32_t)&gdt_ptr);
    tss_flush();
    
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

void set_tss(uint32_t num, uint16_t ss0, uint32_t esp0){
    uint32_t base = (uint32_t) &tss_entry;
    uint32_t limit = base + sizeof(tss_entry);

    set_gdt(num, base, limit, 0xe9, 0x00);
    memset(&tss_entry, 0, sizeof(tss_entry));

    tss_entry.ss0 = ss0;
    tss_entry.esp0 = esp0;
    tss_entry.cs = 0x08 | 0x3;
    tss_entry.ss = tss_entry.ds = tss_entry.es = tss_entry.fs = tss_entry.gs = 0x10 | 0x3;
}