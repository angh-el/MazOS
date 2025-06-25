#include "rtl8139.hpp"


#define RTL8139_VENDOR_ID 0x10EC
#define RTL8139_DEVICE_ID 0x8139

#define RTL8139_REG_COMMAND        0x37
#define RTL8139_REG_MAC            0x00
#define RTL8139_REG_IMR            0x3C
#define RTL8139_REG_ISR            0x3E
#define RTL8139_REG_RBSTART        0x30
#define RTL8139_REG_CAPR           0x38
#define RTL8139_REG_CBR            0x3A
#define RTL8139_REG_RX_CONFIG      0x44
#define RTL8139_REG_TX_CONFIG      0x40
#define RTL8139_CMD_RESET          0x10
#define RTL8139_CMD_RX_ENABLE      0x08
#define RTL8139_CMD_TX_ENABLE      0x04
#define RX_BUFFER_SIZE             8192 + 16 + 1500

static uint8_t mac[6];
static uint8_t* rx_buffer = nullptr;
static uint32_t io_base = 0;

void print_mac() {
    // printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //        mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    printf("MAC Address: %X:%X:%X:%X:%X:%X\n",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void rtl8139_irq_handler(struct interrupt_register* regs) {
    uint16_t status = port_word_in(io_base + RTL8139_REG_ISR);
    port_word_out(io_base + RTL8139_REG_ISR, status); // acknowledge
    if (status & 0x01) {
        rtl8139_receive();
    }
}

void rtl8139_receive() {
    // Simplified example, parsing omitted
    printf("Received a packet (not parsed)\n");
}

void rtl8139_send(void* data, uint32_t len) {
    static int tx_cur = 0;
    uint32_t tx_addr = io_base + 0x20 + (tx_cur * 4);
    uint32_t tx_status = io_base + 0x10 + (tx_cur * 4);
    outl(tx_addr, (uint32_t)data);
    outl(tx_status, len);
    tx_cur = (tx_cur + 1) % 4;
}

void rtl8139_INIT(uint32_t pci_io_base, uint8_t irq_line) {
    io_base = pci_io_base;
    outb(0x00, io_base + 0x52); 
    outb(io_base + RTL8139_REG_COMMAND, RTL8139_CMD_RESET);
    while ((inb(io_base + RTL8139_REG_COMMAND) & RTL8139_CMD_RESET) != 0);
    // printf("yo");

    for (int i = 0; i < 6; i++){
        mac[i] = inb(io_base + RTL8139_REG_MAC + i);
        // printf("%d\n", inb(io_base + RTL8139_REG_MAC + i));
        // printf("%d\n", mac[i]);
    }

    print_mac();

    rx_buffer = (uint8_t*)0x100000; // Just an example addr
    outl(io_base + RTL8139_REG_RBSTART, (uint32_t)rx_buffer);

    outb(io_base + RTL8139_REG_COMMAND, RTL8139_CMD_RX_ENABLE | RTL8139_CMD_TX_ENABLE);
    outl(io_base + RTL8139_REG_RX_CONFIG, 0xf | (1 << 7));
    port_byte_out(io_base + RTL8139_REG_IMR, 0x5); // Enable RX and TX OK interrupts

    // register_interrupt_handler(irq_line, rtl8139_irq_handler);
    DescriptorTables::IDT::irq_install_handler(irq_line, rtl8139_irq_handler);
    printf("RTL8139 initialized at I/O base: 0x%x, IRQ: %d\n", io_base, irq_line);
}

void initialize_rtl8139_with_explicit_params() {
    uint8_t bus, dev, func;

    if (!find_rtl8139(bus, dev, func)) {
        printf("RTL8139 not found\n");
        return;
    }

    // Read I/O base address (BAR0 at offset 0x10)
    uint32_t bar0 = pci_config_read(bus, dev, func, 0x10);
    printf("bar0: %x\n", bar0);
    uint32_t io_base = bar0 & ~0x3;  // mask bottom bits (I/O space indicator)
    if ((bar0 & 0x1) == 0) {
        printf("BAR0 is not an I/O address! Got: 0x%x\n", bar0);
        return;
    }
    io_base = 0xc00c001;

    // Read IRQ line (offset 0x3C)
    uint8_t irq_line = pci_config_read(bus, dev, func, 0x3C) & 0xFF;

    printf("Found RTL8139 at bus %u, device %u, function %u\n", bus, dev, func);
    printf("IO Base: 0x%X, IRQ Line: %u\n", io_base, irq_line);

    rtl8139_INIT(io_base, irq_line);
}