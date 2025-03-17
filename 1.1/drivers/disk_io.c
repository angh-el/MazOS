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
    // ATA read commands
    port_byte_out(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((sector_number >> 24) & 0x0F)); // drive and head
    port_byte_out(ATA_PRIMARY_IO_BASE + 2, 1); // number of sectors
    port_byte_out(ATA_PRIMARY_IO_BASE + 3, sector_number & 0xFF); // sector number (low byte)
    port_byte_out(ATA_PRIMARY_IO_BASE + 4, (sector_number >> 8) & 0xFF); // cylinder low byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 5, (sector_number >> 16) & 0xFF); // cylinder high byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0x20); // read command

    // wait for data to be ready
    // printf("heyy");

    while (!(port_byte_in(ATA_PRIMARY_IO_BASE + 7) & 0x08));
    // int timeout_counter = 0;
    //     while (!(port_byte_in(ATA_PRIMARY_IO_BASE + 7) & 0x08)) { // Check DRQ (Data Request) flag
    //         if (++timeout_counter > 100000000) {
    //             disk_initialize();
    //             read_sector(sector_number, buffer);
    //         }
    //     }
    
    // printf("heyy2");
    // read the sector data
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
//     // drive and head information
//     port_byte_out(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((sector_number >> 24) & 0x0F));

//     // printf("buffer: %s\n", buffer);
//     // printf("sector_number: %d\n", sector_number);
//     // printf("\n", sector_number);

//     // printf("\n");
    
//     // set_cursor(get_cursor() - 80);
//     // printf("\b");
//     // printf("\b");printf("\b");printf("\b");printf("\b");printf("\b");
//     // sector_number = sector_number + 5;
//     // sector_number = sector_number - 5;
//     // sector count (1 sector)
//     port_byte_out(ATA_PRIMARY_IO_BASE + 2, 1);

//     // Logical Block Address sector number
//     port_byte_out(ATA_PRIMARY_IO_BASE + 3, sector_number & 0xFF);         // Low byte
//     port_byte_out(ATA_PRIMARY_IO_BASE + 4, (sector_number >> 8) & 0xFF);  // Mid byte
//     port_byte_out(ATA_PRIMARY_IO_BASE + 5, (sector_number >> 16) & 0xFF); // High byte

//     // WRITE SECTORS command (0x30)
//     port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0x30);
  

//     // uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
//     // while (!(status & 0x08)) { // check the DRQ (Data Request) flag
//     //     if (status & 0x01) {   // check the ERR (Error) flag
//     //         return -1;         // return failure
//     //     }
//     //     status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
//     //     //printf("status %d \n", status); 
//     // }

//     // Wait for drive to be ready
//     uint8_t status;
//     int timeout = 1000000; // Arbitrary large number to prevent infinite loop

//     while (timeout--) {
//         status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);

//         if (!(status & 0x80)) { // Wait for BSY flag to clear first
//             if (status & 0x08) { // DRQ (Data Request) flag is set, we are good
//                 break;
//             }
//             if (status & 0x01) { // ERR (Error) flag is set
//                 printf("ERROR: Disk write failed at sector %d\n", sector_number);
//                 return -1;
//             }
//         }
//     }
//     uint8_t error = port_byte_in(ATA_PRIMARY_IO_BASE + 1);
//     if (error) {
//     printf("ATA Error: 0x%X\n", error);
//         printf("ATA Error: 0x%X\n", error);
//     }

//     // If we exit the loop due to timeout
//     if (timeout <= 0) {
//         printf("ERROR: Timeout while waiting for drive ready at sector %d\n", sector_number);
//         return -1;
//     }


//     // write the sector data (512 bytes = 256 words)
//     for (int i = 0; i < ATA_SECTOR_SIZE / 2; i++) {
//         port_word_out(ATA_PRIMARY_IO_BASE, ((uint16_t *)buffer)[i]); // Write 2 bytes at a time
//     }

//     // flush the cache
//     port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0xE7);

//     // set_cursor(get_cursor() - 80);
//     return 0; 
// }


#define ATA_STATUS_BSY       0x80    // Busy
#define ATA_STATUS_DRQ       0x08    // Data Request
#define ATA_STATUS_ERR       0x01    // Error

// Wait for BSY flag to clear
int ata_wait_bsy() {
    int timeout = 100000;
    while (timeout--) {
        uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
        if (!(status & ATA_STATUS_BSY)) return 0;  // BSY cleared
    }
    return -1;  // Timeout
}

// Wait for DRQ flag to be set (ready for data transfer)
int ata_wait_drq() {
    int timeout = 100000;
    while (timeout--) {
        uint8_t status = port_byte_in(ATA_PRIMARY_IO_BASE + 7);
        if (status & ATA_STATUS_ERR) return -1;  // Error occurred
        if (status & ATA_STATUS_DRQ) return 0;   // DRQ is set
    }
    return -1;  // Timeout
}

int write_sector(uint32_t sector_number, const uint8_t *buffer) {
    // Select the drive and enable LBA mode
    port_byte_out(ATA_PRIMARY_IO_BASE + 6, 0xE0 | ((sector_number >> 24) & 0x0F));

    // Set the sector count (always 1 sector)
    port_byte_out(ATA_PRIMARY_IO_BASE + 2, 1);
    
    // Set LBA sector number (split into 3 bytes)
    port_byte_out(ATA_PRIMARY_IO_BASE + 3, sector_number & 0xFF);         // Low byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 4, (sector_number >> 8) & 0xFF);  // Mid byte
    port_byte_out(ATA_PRIMARY_IO_BASE + 5, (sector_number >> 16) & 0xFF); // High byte

    // Send WRITE SECTORS command (0x30)
    port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0x30);

    // Wait for drive to be ready (BSY must clear first)
    if (ata_wait_bsy() < 0) {
        printf("ERROR: Timeout waiting for BSY to clear\n");
        return -1;
    }

    // Wait for drive to request data transfer (DRQ must be set)
    if (ata_wait_drq() < 0) {
        printf("ERROR: Timeout waiting for DRQ\n");
        return -1;
    }

    // Write 512 bytes (256 words) to the sector
    for (int i = 0; i < ATA_SECTOR_SIZE / 2; i++) {
        port_word_out(ATA_PRIMARY_IO_BASE, ((uint16_t *)buffer)[i]);
    }

    // Send CACHE FLUSH command (0xE7) to ensure data is written
    port_byte_out(ATA_PRIMARY_IO_BASE + 7, 0xE7);

    // Wait for BSY to clear after flush
    if (ata_wait_bsy() < 0) {
        printf("ERROR: Timeout after flush\n");
        return -1;
    }

    return 0;  // Success
}




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



int read_bytes(uint32_t start_byte, uint32_t num_bytes, uint8_t *buffer) {
    uint8_t sector_buffer[ATA_SECTOR_SIZE]; // Temporary buffer to hold sector data
    uint32_t start_sector = start_byte / ATA_SECTOR_SIZE; // Starting sector
    uint32_t end_byte = start_byte + num_bytes; // Last byte to read
    uint32_t end_sector = end_byte / ATA_SECTOR_SIZE; // Last sector involved
    uint32_t offset_in_sector = start_byte % ATA_SECTOR_SIZE; // Offset within the first sector

    uint32_t bytes_read = 0;

    for (uint32_t sector = start_sector; sector <= end_sector; sector++) {
        // Read the current sector into the temporary buffer
        if (read_sector(sector, sector_buffer) != 0) {
            return -1; // Return error if sector read fails
        }

        // Calculate the range of bytes to copy from this sector
        uint32_t start = (sector == start_sector) ? offset_in_sector : 0;
        uint32_t end = (sector == end_sector) ? (end_byte % ATA_SECTOR_SIZE) : ATA_SECTOR_SIZE;

        uint32_t bytes_to_copy = end - start;

        // Copy the relevant portion to the destination buffer manually
        for (uint32_t i = 0; i < bytes_to_copy; i++) {
            buffer[bytes_read + i] = sector_buffer[start + i];
        }

        bytes_read += bytes_to_copy;

        // If we've read the required number of bytes, we're done
        if (bytes_read >= num_bytes) {
            break;
        }
    }

    return bytes_read; // Return the number of bytes read
}
