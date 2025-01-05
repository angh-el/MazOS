#include "idt.h"
#include "drivers/screen.h"
#include "libs/util.h"

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr;

extern void idt_flush(uint32_t);


void init_idt(){
    idt_ptr.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr.base = (uint32_t) &idt_entries;

    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);

    port_byte_out(0x20, 0x11);
    port_byte_out(0xA0, 0x11);
    port_byte_out(0x21, 0x20);
    port_byte_out(0xA1, 0x28);
    port_byte_out(0x21,0x04);
    port_byte_out(0xA1,0x02);
    port_byte_out(0x21, 0x01);
    port_byte_out(0xA1, 0x01);
    port_byte_out(0x21, 0x0);
    port_byte_out(0xA1, 0x0);

    set_idt(0, (uint32_t)isr0,0x08, 0x8E);
    set_idt(1, (uint32_t)isr1,0x08, 0x8E);
    set_idt(2, (uint32_t)isr2,0x08, 0x8E);
    set_idt(3, (uint32_t)isr3,0x08, 0x8E);
    set_idt(4, (uint32_t)isr4, 0x08, 0x8E);
    set_idt(5, (uint32_t)isr5, 0x08, 0x8E);
    set_idt(6, (uint32_t)isr6, 0x08, 0x8E);
    set_idt(7, (uint32_t)isr7, 0x08, 0x8E);
    set_idt(8, (uint32_t)isr8, 0x08, 0x8E);
    set_idt(9, (uint32_t)isr9, 0x08, 0x8E);
    set_idt(10, (uint32_t)isr10, 0x08, 0x8E);
    set_idt(11, (uint32_t)isr11, 0x08, 0x8E);
    set_idt(12, (uint32_t)isr12, 0x08, 0x8E);
    set_idt(13, (uint32_t)isr13, 0x08, 0x8E);
    set_idt(14, (uint32_t)isr14, 0x08, 0x8E);
    set_idt(15, (uint32_t)isr15, 0x08, 0x8E);
    set_idt(16, (uint32_t)isr16, 0x08, 0x8E);
    set_idt(17, (uint32_t)isr17, 0x08, 0x8E);
    set_idt(18, (uint32_t)isr18, 0x08, 0x8E);
    set_idt(19, (uint32_t)isr19, 0x08, 0x8E);
    set_idt(20, (uint32_t)isr20, 0x08, 0x8E);
    set_idt(21, (uint32_t)isr21, 0x08, 0x8E);
    set_idt(22, (uint32_t)isr22, 0x08, 0x8E);
    set_idt(23, (uint32_t)isr23, 0x08, 0x8E);
    set_idt(24, (uint32_t)isr24, 0x08, 0x8E);
    set_idt(25, (uint32_t)isr25, 0x08, 0x8E);
    set_idt(26, (uint32_t)isr26, 0x08, 0x8E);
    set_idt(27, (uint32_t)isr27, 0x08, 0x8E);
    set_idt(28, (uint32_t)isr28, 0x08, 0x8E);
    set_idt(29, (uint32_t)isr29, 0x08, 0x8E);
    set_idt(30, (uint32_t)isr30, 0x08, 0x8E);
    set_idt(31, (uint32_t)isr31, 0x08, 0x8E);
    set_idt(32, (uint32_t)irq0, 0x08, 0x8E);
    set_idt(33, (uint32_t)irq1, 0x08, 0x8E);
    set_idt(34, (uint32_t)irq2, 0x08, 0x8E);
    set_idt(35, (uint32_t)irq3, 0x08, 0x8E);
    set_idt(36, (uint32_t)irq4, 0x08, 0x8E);
    set_idt(37, (uint32_t)irq5, 0x08, 0x8E);
    set_idt(38, (uint32_t)irq6, 0x08, 0x8E);
    set_idt(39, (uint32_t)irq7, 0x08, 0x8E);
    set_idt(40, (uint32_t)irq8, 0x08, 0x8E);
    set_idt(41, (uint32_t)irq9, 0x08, 0x8E);
    set_idt(42, (uint32_t)irq10, 0x08, 0x8E);
    set_idt(43, (uint32_t)irq11, 0x08, 0x8E);
    set_idt(44, (uint32_t)irq12, 0x08, 0x8E);
    set_idt(45, (uint32_t)irq13, 0x08, 0x8E);
    set_idt(46, (uint32_t)irq14, 0x08, 0x8E);
    set_idt(47, (uint32_t)irq15, 0x08, 0x8E);
    set_idt(128, (uint32_t)isr128, 0x08, 0x8E);     // sys calls
    set_idt(177, (uint32_t)isr177, 0x08, 0x8E);     // sys calls


    idt_flush((uint32_t)&idt_ptr);

}

void set_idt(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags){
    idt_entries[num].base_low = base & 0xffff;
    idt_entries[num].base_high = (base >> 16) & 0xffff;
    idt_entries[num].selector = selector;
    idt_entries[num].flags = flags | 0x60;
    idt_entries[num].zero = 0;
}

unsigned char* interrupt_messages[] ={
    "Division by zero",
    "Single-step interrupt",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Coprocessor not available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid Task State Segment",
    "Segment not present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating Point Exception",
    "Alignment Fault",
    "Machine Check", 
    "Single Instruction Multiple Data Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void handle_isr(struct interrupt_register* registers){
    if(registers->interrupt_num < 32){
        print(interrupt_messages[registers->interrupt_num]);
        print("\nSystem is on a break styulll\n");

        // pic_acknowledge(registers->interrupt_num);
        for(;;);
    }   
}

void *irq_routines[16] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

void irq_install_handler (int irq, void (*handler)(struct interrupt_register *reg)){
    irq_routines[irq] = handler;
}

void clear_irq_handler(int irq){
    irq_routines[irq] = 0;
}

void handle_irq(struct interrupt_register *registers){
    void(*handler)(struct interrupt_register *registers);
    handler = irq_routines[registers->interrupt_num - 32];
    
    if(handler) handler(registers);

    if(registers->interrupt_num >= 40) port_byte_out(0xa0, 0x20);

    port_byte_out(0x20, 0x20);
}