#include "disk_io.h"
#include "../libs/util.h"


// int read_sector(uint32_t sector_number, uint8_t *buffer) {
//     // Call BIOS interrupt to read the sector
//     // This is a placeholder; real implementation depends on your setup
//     asm volatile(
//         "int $0x13"              // BIOS disk interrupt
//         :                        // No output operands
//         : "a"(0x0201),           // AH=02h (read sectors), AL=01h (number of sectors)
//           "c"((sector_number & 0xFF00) >> 8), // CH=cylinder high byte, CL=low byte
//           "d"(sector_number & 0xFF),          // DH=track/head, DL=drive number
//           "D"(buffer)           // ES:BX points to buffer
//         : "memory");
//     return 0; // Success
// }




#define ATA_PRIMARY_IO_BASE 0x1F0 // Base I/O port for the primary ATA channel
#define ATA_SECTOR_SIZE 512
#define ATA_PRIMARY_CTRL_BASE 0x3F6 // Primary ATA control base


int disk_initialize(void) {
    // Reset the disk controller
    port_byte_out(ATA_PRIMARY_CTRL_BASE, 0x04);  // Set reset bit
    port_byte_out(ATA_PRIMARY_CTRL_BASE, 0x00);  // Clear reset bit

    // Wait for the disk to be ready
    uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7); // Read status register
    while (status & 0x80) { // Check the BSY (Busy) flag
        status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
    }

    // Return success (you could add more checks, e.g., for device presence)
    return 0;
}



int read_sector(uint32_t sector_number, uint8_t *buffer) {
    // Send commands to the ATA controller to read the sector
    port_byte_out(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((sector_number >> 24) & 0x0F)); // Drive and head
    port_byte_out(ATA_PRIMARY_IO_BASE + 2, 1); // Number of sectors
    port_byte_out(ATA_PRIMARY_IO_BASE + 3, sector_number & 0xFF); // Sector number (low byte)
    port_byte_out(ATA_PRIMARY_IO_BASE + 4, (sector_number >> 8) & 0xFF); // Cylinder low byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 5, (sector_number >> 16) & 0xFF); // Cylinder high byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0x20); // Read command

    // Wait for the data to be ready
    while (!(port_byte_in(ATA_PRIMARY_IO_BASE + 7) & 0x08));

    // Read the sector data
    for (int i = 0; i < ATA_SECTOR_SIZE / 2; i++) {
        ((uint16_t *)buffer)[i] = port_word_in(ATA_PRIMARY_IO_BASE);
    }


    // // Wait for the data to be ready
    // uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
    // while (!(status & 0x08)) { // Check the DRQ (Data Request) flag
    //     if (status & 0x01) {   // Check the ERR (Error) flag
    //         return -1;         // Return failure
    //     }
    //     status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
    // }
    // // Read the sector data into the buffer (512 bytes = 256 words)
    // for (int i = 0; i < ATA_SECTOR_SIZE / 2; i++) {
    //     ((uint16_t *)buffer)[i] = port_word_in(ATA_PRIMARY_IO_BASE); // Read 2 bytes at a time
    // }



    return 0;
}




// int write_sector(uint32_t sector_number, const uint8_t *buffer) {
//     // Send the drive and head information
//     outb(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((sector_number >> 24) & 0x0F));

//     // Send the sector count (1 sector)
//     outb(ATA_PRIMARY_IO_BASE + 2, 1);

//     // Send the LBA (Logical Block Address) sector number
//     outb(ATA_PRIMARY_IO_BASE + 3, sector_number & 0xFF);         // Low byte
//     outb(ATA_PRIMARY_IO_BASE + 4, (sector_number >> 8) & 0xFF);  // Mid byte
//     outb(ATA_PRIMARY_IO_BASE + 5, (sector_number >> 16) & 0xFF); // High byte

//     // Send the WRITE SECTORS command (0x30)
//     outb(ATA_PRIMARY_IO_BASE + 7, 0x30);

//     // Wait for the drive to be ready
//     uint8_t status = inb(ATA_PRIMARY_IO_BASE + 7);
//     while (!(status & 0x08)) { // Check the DRQ (Data Request) flag
//         if (status & 0x01) {   // Check the ERR (Error) flag
//             return -1;         // Return failure
//         }
//         status = inb(ATA_PRIMARY_IO_BASE + 7);
//     }

//     // Write the sector data (512 bytes = 256 words)
//     for (int i = 0; i < ATA_SECTOR_SIZE / 2; i++) {
//         outw(ATA_PRIMARY_IO_BASE, ((uint16_t *)buffer)[i]); // Write 2 bytes at a time
//     }

//     // Flush the cache (optional)
//     outb(ATA_PRIMARY_IO_BASE + 7, 0xE7);

//     return 0; // Success
// }






uint32_t get_disk_size(void) {
    uint8_t buffer[512];

    // Send the IDENTIFY DEVICE command
    port_byte_out(ATA_PRIMARY_IO_BASE + 6, 0xA0);  // Select drive
    port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0xEC);  // IDENTIFY command

    // Wait for the data to be ready
    uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
    while (!(status & 0x08)) { // Check the DRQ flag
        if (status & 0x01) {   // Check the ERR flag
            return -1;         // Return failure
        }
        status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
    }

    // Read the IDENTIFY DEVICE data (512 bytes)
    for (int i = 0; i < 256; i++) {
        ((uint16_t *)buffer)[i] = port_word_in(ATA_PRIMARY_IO_BASE);
    }

    // Total number of sectors is at word 60 and 61 (Little Endian)
    uint32_t total_sectors = buffer[120] | (buffer[121] << 8) |
                             (buffer[122] << 16) | (buffer[123] << 24);

    return total_sectors;
}

