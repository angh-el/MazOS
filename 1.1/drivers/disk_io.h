#ifndef DISK_IO_H
#define DISK_IO_H

#include "../libs/stdint.h"

// Function prototypes for disk I/O
/**
 * Reads a sector from the disk.
 * @param sector_number The sector number to read.
 * @param buffer A pointer to the buffer where the data will be stored.
 * @return 0 on success, -1 on failure.
 */
int read_sector(uint32_t sector_number, uint8_t *buffer);

/**
 * Writes a sector to the disk.
 * @param sector_number The sector number to write to.
 * @param buffer A pointer to the buffer containing the data to write.
 * @return 0 on success, -1 on failure.
 */
int write_sector(uint32_t sector_number, const uint8_t *buffer);

/**
 * Initializes the disk driver.
 * This function is called during OS initialization to set up the disk driver.
 * @return 0 on success, -1 on failure.
 */
int disk_initialize(void);

/**
 * Gets the total number of sectors on the disk.
 * This function retrieves the disk's size in sectors.
 * @return Total number of sectors, or -1 if an error occurs.
 */
uint32_t get_disk_size(void);

#endif // DISK_IO_H
