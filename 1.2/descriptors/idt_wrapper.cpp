#include "idt.hpp"

extern "C" void handle_isr(struct interrupt_register* r) {
    DescriptorTables::IDT::handle_isr(r);
}

extern "C" void handle_irq(struct interrupt_register* r) {
    DescriptorTables::IDT::handle_irq(r);
}
