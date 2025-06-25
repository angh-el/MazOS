#include "pci.hpp"


#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA    0xCFC

uint32_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t address =
        (1U << 31) |               // enable bit
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC);           // align to 4 bytes
    outl(CONFIG_ADDRESS, address);
    return inl(CONFIG_DATA);
}

void pci_config_write(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address =
        (1U << 31) |
        ((uint32_t)bus << 16) |
        ((uint32_t)device << 11) |
        ((uint32_t)function << 8) |
        (offset & 0xFC);
    outl(CONFIG_ADDRESS, address);
    outl(CONFIG_DATA, value);
}

uint16_t pci_config_read16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset) {
    uint32_t val = pci_config_read(bus, device, function, offset & 0xFC);
    return (val >> ((offset & 2) * 8)) & 0xFFFF;
}

void pci_config_write16(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset, uint16_t value) {
    uint32_t old = pci_config_read(bus, device, function, offset & 0xFC);
    uint32_t shift = (offset & 2) * 8;
    uint32_t mask = 0xFFFF << shift;
    uint32_t newval = (old & ~mask) | ((value << shift) & mask);
    pci_config_write(bus, device, function, offset & 0xFC, newval);
}




/////////////////////////////////////////////////
// configs the rtl8139

#define RTL8139_VENDOR 0x10EC
#define RTL8139_DEVICE 0x8139

uint16_t io_base;

// Scan PCI bus for RTL8139
bool find_rtl8139(uint8_t &bus, uint8_t &dev, uint8_t &func) {
    for (bus = 0; bus < 256; ++bus)
    for (dev = 0; dev < 32; ++dev)
    for (func = 0; func < 8; ++func) {
    // printf("%d ",bus);
        uint32_t d = pci_config_read(bus, dev, func, 0x0);
        if ((d & 0xFFFF) == RTL8139_VENDOR && ((d >> 16) & 0xFFFF) == RTL8139_DEVICE)
            return true;
    }
    return false;
}

// Initialize RTL8139
bool rtl8139_init() {
    uint8_t bus, dev, func;
    if (!find_rtl8139(bus, dev, func)) return false;

    // Enable bus mastering
    uint16_t cmd = pci_config_read16(bus, dev, func, 0x04);
    pci_config_write16(bus, dev, func, 0x04, cmd | 0x0004);

    // Read BAR0
    uint32_t bar0 = pci_config_read(bus, dev, func, 0x10);
    io_base = bar0 & ~0x3;

    // Power up chip
    outb(0x00, io_base + 0x52);

    // Software reset: write 0x10, wait until bit clears
    outb(0x10, io_base + 0x37);
    while (inb(io_base + 0x37) & 0x10);

    // Enable TX (bit 3) and RX (bit 2)
    outb(0x0C, io_base + 0x37);
    

    return true;
}