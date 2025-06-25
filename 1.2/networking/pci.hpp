
#ifndef PCI_H
#define PCI_H

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"
#include "../lib/util.hpp"

inline void outb(uint8_t val, uint16_t port) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void outl(uint16_t port, uint32_t val) {
    asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_config_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value);

uint16_t pci_config_read16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
void pci_config_write16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value);

bool rtl8139_init();
bool find_rtl8139(uint8_t &bus, uint8_t &dev, uint8_t &func);


#endif
