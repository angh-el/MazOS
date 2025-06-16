#ifndef IDT_HPP
#define IDT_HPP

#include "../lib/stdint.hpp"
#include "../lib/util.hpp"


namespace DescriptorTables {
    // structs
    struct IDTEntry{
        uint16_t base_low;      // offset bits 0..15
        uint16_t selector;      // a code segment selector in GDT or LDT
        uint8_t zero;           // unused, set to 0  
        uint8_t flags;          // gate type, dpl, and p fields
        uint16_t base_high;     // offset bits 16..31
    }__attribute__((packed));

    struct IDTPtr{
        uint16_t limit;
        uint32_t base;
    }__attribute__((packed));

    char* interrupt_messages[] ={
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

    // class
    class IDT{
    public:
        static void init();
        static void handle_isr(struct interrupt_register* registers);
        static void irq_install_handler (int irq, void (*handler)(struct interrupt_register *r));
        static void clear_irq_handler(int irq);
        static void handle_irq(struct interrupt_register *regs);
        static void print_irq_routine(int irq);
    
    private:
        static void set_idt(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags);

    };

    extern void isr0();
    extern void isr1();
    extern void isr2();
    extern void isr3();
    extern void isr4();
    extern void isr5();
    extern void isr6();
    extern void isr7();
    extern void isr8();
    extern void isr9();
    extern void isr10();
    extern void isr11();
    extern void isr12();
    extern void isr13();
    extern void isr14();
    extern void isr15();
    extern void isr16();
    extern void isr17();
    extern void isr18();
    extern void isr19();
    extern void isr20();
    extern void isr21();
    extern void isr22();
    extern void isr23();
    extern void isr24();
    extern void isr25();
    extern void isr26();
    extern void isr27();
    extern void isr28();
    extern void isr29();
    extern void isr30();
    extern void isr31();
    extern void isr128();
    extern void isr177();
    extern void irq0();
    extern void irq1();
    extern void irq2();
    extern void irq3();
    extern void irq4();
    extern void irq5();
    extern void irq6();
    extern void irq7();
    extern void irq8();
    extern void irq9();
    extern void irq10();
    extern void irq11();
    extern void irq12();
    extern void irq13();
    extern void irq14();
    extern void irq15();


}


#endif