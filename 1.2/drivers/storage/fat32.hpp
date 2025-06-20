#ifndef FAT32_HPP
#define FAT32_HPP

// library includes
#include "../../lib/stdint.hpp"
#include "../../lib/util.hpp"
#include "../../lib/printf.hpp"
#include "../../drivers/display/display.hpp"
#include "../../drivers/display/graphics.hpp"

#include "disk.hpp"

struct FAT32_BootSector {
    uint8_t jump_code[3];             // 0x00 - Jump instruction (to execute the bootloader)
    char oem_name[8];                 // 0x03 - OEM name (e.g., "MSWIN4.1")
    uint16_t bytes_per_sector;        // 0x0B - Bytes per sector (typically 512 or 1024)
    uint8_t sectors_per_cluster;      // 0x0D - Sectors per cluster (1, 2, 4, 8, 16, 32, 64, 128)
    uint16_t reserved_sectors;        // 0x0E - Reserved sectors (usually 32)
    uint8_t num_fats;                 // 0x10 - Number of FAT tables (usually 2)
    uint16_t root_entries;            // 0x11 - Root directory entries (not used in FAT32)
    uint16_t total_sectors_16;        // 0x13 - Total sectors (for FAT16, usually 0 in FAT32)
    uint8_t media_descriptor;         // 0x15 - Media descriptor (usually 0xF8 for fixed drives)
    uint16_t sectors_per_fat_16;      // 0x16 - Sectors per FAT (for FAT16, usually 0 in FAT32)
    uint16_t sectors_per_track;       // 0x18 - Sectors per track (physical disk geometry)
    uint16_t num_heads;               // 0x1A - Number of heads (physical disk geometry)
    uint32_t hidden_sectors;          // 0x1C - Hidden sectors (sectors before the partition)
    uint32_t total_sectors_32;        // 0x20 - Total sectors (for FAT32)
    
    // FAT32 specific fields:
    uint32_t sectors_per_fat;         // 0x24 - Sectors per FAT
    uint16_t ext_flags;               // 0x28 - Extended flags
    uint16_t fs_version;              // 0x2A - File system version (usually 0x00F0)
    uint32_t root_cluster;            // 0x2C - Root directory cluster (start of root directory)
    uint16_t fs_info;                 // 0x30 - FSINFO sector number (typically 1 or 2)
    uint16_t backup_boot_sector;      // 0x32 - Backup boot sector (typically 6 or 7)
    
    uint8_t reserved[12];             // 0x34 - Reserved for future use
    uint8_t drive_number;             // 0x40 - Physical drive number (0x80 for hard disk, 0x00 for others)
    uint8_t reserved2;                // 0x41 - Reserved
    uint8_t boot_signature;           // 0x42 - Boot signature (0x29 for valid signature)
    uint32_t volume_id;               // 0x43 - Volume ID (unique identifier)
    char volume_label[11];            // 0x47 - Volume label (e.g., "NO NAME    ")
    char fs_type[8];                  // 0x52 - File system type (e.g., "FAT32   ")
    
    uint8_t boot_code[420];           // 0x5A - Boot code (can be the bootloader code)
    uint16_t boot_sector_signature;   // 0x1FE - 0x55AA (boot sector signature)
} __attribute__((packed));


struct FAT32_DirectoryEntry {
    uint8_t name[11];
    uint8_t attributes;
    uint8_t reserved;
    uint8_t creation_time_tenth;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t write_time;
    uint16_t write_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed));



class Fat32 {
public:
    int mount(uint32_t sector_number);
    void read_root_directory();
    void delete_entry(const char *filename);
    void read_file(const char *filename);
    void draw_png_from_txt(const char *filename);
    void ls();
    void get_current_directory();
    void cd(const char *dirname);
    void cd_up();

private:
    // varaibles
    struct FAT32_BootSector boot_sector;
    int fat32_start_sector;
    uint32_t current_directory_cluster;
    uint32_t root_directory_cluster;

    // functions
    uint32_t get_next_cluster(uint32_t current_cluster);
    uint32_t cluster_to_sector(uint32_t cluster);
    uint32_t get_file_entry(const char *filename);
    boolean find_directory_name(uint32_t search_cluster, uint32_t target_cluster, char *name_buffer);
    uint32_t read_fat_entry(uint32_t cluster_number);
    struct FAT32_DirectoryEntry* find_free_directory_entry(uint8_t* buffer);
    uint32_t find_free_cluster();
    void write_fat_entry(uint32_t cluster, uint32_t value);
    int create_entry(const char* name, uint8_t attributes);
    int update_file_size(const char *filename, uint32_t new_size);
    int overwrite_file(const uint8_t *data, uint32_t data_length, const char *filename);
    int append_to_file(uint32_t file_start_cluster, const uint8_t *data, uint32_t data_length, uint32_t file_size , const char *filename);
};


#endif