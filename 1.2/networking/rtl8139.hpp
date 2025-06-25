#ifndef RTL8139_HPP
#define RTL8139_HPP

#include "pci.hpp"
#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"
#include "../lib/util.hpp"

#include "../descriptors/idt.hpp"

void print_mac();
void rtl8139_irq_handler(struct interrupt_register* regs);
void rtl8139_receive();
void rtl8139_send(void* data, uint32_t len);
void rtl8139_INIT(uint32_t pci_io_base, uint8_t irq_line);

void initialize_rtl8139_with_explicit_params();

#endif