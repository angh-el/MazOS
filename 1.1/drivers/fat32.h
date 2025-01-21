#pragma once

#include "../libs/stdint.h"

// Boot sector offsets (relative to the start of the sector)
#define BS_OEM_NAME 3
#define BS_BYTES_PER_SECTOR 11
#define BS_SECTORS_PER_CLUSTER 13
#define BS_RESERVED_SECTORS 14
#define BS_NUM_FATS 16
#define BS_ROOT_CLUSTER 44

// A sector is typically 512 bytes
#define SECTOR_SIZE 512

// FAT32 Directory Entry Attributes
#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20

// FAT32 Directory Entry structure
struct FAT32_DirectoryEntry{
    char name[11];          // File name in 8.3 format
    uint8_t attributes;     // File attributes
    uint8_t reserved;       // Reserved
    uint8_t creation_time;  // Creation time (fine resolution)
    uint16_t creation_date; // Creation date
    uint16_t last_access_date; // Last access date
    uint16_t first_cluster_high; // High word of first cluster
    uint16_t write_time;    // Last write time
    uint16_t write_date;    // Last write date
    uint16_t first_cluster_low; // Low word of first cluster
    uint32_t file_size;     // File size in bytes
} __attribute__((packed)) ;

// Function prototypes
void fat32_initialize(void);
uint32_t fat32_get_root_cluster(void);
void fat32_read_cluster(uint32_t cluster, uint8_t *buffer);
uint32_t fat32_get_next_cluster(uint32_t cluster);
int fat32_find_file(const char *filename, FAT32_DirectoryEntry *entry);
int fat32_read_file(const char *filename, uint8_t *buffer, uint32_t size);


