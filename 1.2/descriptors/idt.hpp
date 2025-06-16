#ifndef IDT_HPP
#define IDT_HPP

#include "../lib/stdint.hpp"
#include "../lib/util.hpp"


extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
extern "C" void isr128();
extern "C" void isr177();
extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();



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

}


#endif