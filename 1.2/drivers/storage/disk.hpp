#ifndef DISK_HPP
#define DISK_HPP

// external includes
#include "../../lib/stdint.hpp"
#include "../../lib/util.hpp"
#include "../../lib/printf.hpp"

#define ATA_SECTOR_SIZE 512

#define ATA_PRIMARY_IO_BASE 0x1F0 // Base I/O port for the primary ATA channel
#define ATA_PRIMARY_CTRL_BASE 0x3F6 // Primary ATA control base

#define ATA_STATUS_BSY       0x80    // Busy
#define ATA_STATUS_DRQ       0x08    // Data Request
#define ATA_STATUS_ERR       0x01    // Error

class Disk{
public:
    static int init();
    static int read_sector(uint32_t sector_number, uint8_t *buffer);
    static int write_sector(uint32_t sector_number, const uint8_t *buffer);
    static int read_bytes(uint32_t start_byte, uint32_t num_bytes, uint8_t *buffer);
    static uint32_t get_size();

private:
    static int wait_bsy();
    static int wait_drq();

};




#endif