// #include <stdio.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>

// // Define the FAT32 Boot Sector structure (simplified)
// struct FAT32_BootSector {
//     uint8_t jump_code[3];            // 0x00 - Jump instruction
//     char oem_name[8];                // 0x03 - OEM name
//     uint16_t bytes_per_sector;       // 0x0B - Bytes per sector
//     uint8_t sectors_per_cluster;     // 0x0D - Sectors per cluster
//     uint16_t reserved_sectors;       // 0x0E - Reserved sectors
//     uint8_t num_fats;                // 0x10 - Number of FAT tables
//     uint32_t sectors_per_fat;        // 0x24 - Sectors per FAT
//     uint32_t root_cluster;           // 0x2C - Root directory cluster
//     uint16_t fs_info;                // 0x30 - FSInfo sector number (for FAT32)
//     uint16_t backup_boot_sector;     // 0x32 - Backup boot sector
//     // Other fields can be added based on your needs
// } __attribute__((packed));

// // FAT32 Directory Entry Structure
// struct FAT32_DirectoryEntry {
//     char name[11];                   // 0x00 - File name in 8.3 format
//     uint8_t attr;                    // 0x0B - File attributes
//     uint8_t reserved;                // 0x0C - Reserved
//     uint8_t creation_time_tenths;    // 0x0D - Creation time in tenths of a second
//     uint16_t creation_time;          // 0x0E - Creation time
//     uint16_t creation_date;          // 0x10 - Creation date
//     uint16_t last_access_date;       // 0x12 - Last access date
//     uint16_t first_cluster_high;     // 0x14 - High word of first cluster number
//     uint16_t last_mod_time;          // 0x16 - Last modified time
//     uint16_t last_mod_date;          // 0x18 - Last modified date
//     uint16_t first_cluster_low;      // 0x1A - Low word of first cluster number
//     uint32_t file_size;              // 0x1C - File size in bytes
// } __attribute__((packed));





// // Simple memory copy function
// void simple_memcpy(void* dest, const void* src, size_t n) {
//     uint8_t* d = (uint8_t*) dest;
//     const uint8_t* s = (const uint8_t*) src;
//     for (size_t i = 0; i < n; i++) {
//         d[i] = s[i];
//     }
// }

// // Function to read a sector from the disk image
// int read_sector(FILE *img_file, uint32_t sector_number, uint8_t *buffer) {
//     // Set the file pointer to the correct position (sector_number * sector_size)
//     fseek(img_file, sector_number * 512, SEEK_SET);
    
//     // Read the sector (512 bytes) into the buffer
//     size_t bytes_read = fread(buffer, 1, 512, img_file);
//     if (bytes_read != 512) {
//         return -1;  // Error reading the sector
//     }

//     return 0;  // Success
// }


// // Convert cluster number to sector number
// uint32_t cluster_to_sector(uint32_t cluster, struct FAT32_BootSector *boot_sector) {
//     return boot_sector->reserved_sectors +
//            (boot_sector->num_fats * boot_sector->sectors_per_fat) +
//            ((cluster - 2) * boot_sector->sectors_per_cluster);
// }






// // Function to mount the FAT32 filesystem and print boot sector information
// int fat32_mount(const char *image_path) {
//     FILE *img_file = fopen(image_path, "rb");
//     if (!img_file) {
//         perror("Error opening image file");
//         return -1;
//     }

//     struct FAT32_BootSector boot_sector;
//     {
//         /* data */
//     };
    
//     // Read the boot sector (sector 0)
//     uint8_t buffer[512];
//     if (read_sector(img_file, 0, buffer) != 0) {
//         printf("Error reading boot sector!\n");
//         fclose(img_file);
//         return -1;
//     }

//     // Copy boot sector information into our structure
    
//     simple_memcpy(&boot_sector, buffer, sizeof(struct FAT32_BootSector));

//     // Validate the boot sector by checking some key fields
//     if (boot_sector.bytes_per_sector == 0 || boot_sector.sectors_per_cluster == 0) {
//         printf("Invalid boot sector data!\n");
//         fclose(img_file);
//         return -1;
//     }

//     // Print out the boot sector information
//     printf("FAT32 Boot Sector Information:\n");
//     // printf("OEM Name: %.8s\n", boot_sector.oem_name);
//     printf("OEM Name: %s\n", boot_sector.oem_name);
//     printf("Bytes per sector: %d\n", boot_sector.bytes_per_sector);
//     printf("Sectors per cluster: %d\n", boot_sector.sectors_per_cluster);
//     printf("Reserved sectors: %d\n", boot_sector.reserved_sectors);
//     printf("Number of FATs: %d\n", boot_sector.num_fats);
//     printf("Sectors per FAT: %d\n", boot_sector.sectors_per_fat);
//     printf("Root Cluster: %u\n", boot_sector.root_cluster);
//     printf("FSInfo sector: %u\n", boot_sector.fs_info);
//     printf("Backup boot sector: %u\n", boot_sector.backup_boot_sector);

//     // Optionally, read the root directory or other filesystem structures here

//     // Close the image file
//     fclose(img_file);
//     return 0;
// }

// int main() {
//     const char *image_path = "filesystem.img";  // Path to your FAT32 filesystem image
//     if (fat32_mount(image_path) != 0) {
//         printf("FAT32 mount failed!\n");
//         return -1;
//     }

//     printf("FAT32 filesystem mounted successfully.\n");

//     // print_root_directory(image_path, )
//     return 0;
// }







#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Assuming these are already defined:
struct FAT32_BootSector {
    uint8_t jump_code[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t num_fats;
    uint32_t sectors_per_fat;
    uint32_t root_cluster;
    uint16_t fs_info;
    uint16_t backup_boot_sector;
} __attribute__((packed));

struct FAT32_DirectoryEntry {
    char name[11];
    uint8_t attr;
    uint8_t reserved;
    uint8_t creation_time_tenths;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_high;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint16_t first_cluster_low;
    uint32_t file_size;
} __attribute__((packed));

// Simple memory copy function
void simple_memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*) dest;
    const uint8_t* s = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

// Function to read a sector from the disk image
int read_sector(FILE *img_file, uint32_t sector_number, uint8_t *buffer) {
    fseek(img_file, sector_number * 512, SEEK_SET);
    size_t bytes_read = fread(buffer, 1, 512, img_file);
    if (bytes_read != 512) {
        return -1;  // Error reading the sector
    }
    return 0;  // Success
}

// Convert cluster number to sector number
uint32_t cluster_to_sector(uint32_t cluster, struct FAT32_BootSector *boot_sector) {
    return boot_sector->reserved_sectors +
           (boot_sector->num_fats * boot_sector->sectors_per_fat) +
           ((cluster - 2) * boot_sector->sectors_per_cluster);
}

// Function to get the next cluster from the FAT (this assumes you're working with a FAT32 format)
uint32_t get_next_cluster(FILE *img_file, struct FAT32_BootSector *boot_sector, uint32_t current_cluster) {
    uint32_t fat_offset = current_cluster * 4;  // 4 bytes per entry in FAT32
    uint32_t fat_sector = boot_sector->reserved_sectors + (fat_offset / 512);
    uint32_t fat_entry_offset = fat_offset % 512;

    uint8_t buffer[512];
    if (read_sector(img_file, fat_sector, buffer) != 0) {
        return 0xFFFFFFFF;  // Error reading FAT, return invalid cluster
    }

    uint32_t next_cluster;
    memcpy(&next_cluster, &buffer[fat_entry_offset], 4);
    return next_cluster & 0x0FFFFFFF;  // Mask out the upper bits
}


void print_root_directory(FILE *img_file, struct FAT32_BootSector *boot_sector) {
    uint8_t buffer[512];
    uint32_t current_cluster = boot_sector->root_cluster;

    printf("\nRoot Directory Contents:\n");
    printf("%-12s %-8s %-10s %-10s\n", "Filename", "Type", "Start Cluster", "Size (Bytes)");
    printf("------------------------------------------------\n");

    while (current_cluster < 0x0FFFFFF8) { // Valid cluster range
        // Get the sector corresponding to the current cluster
        uint32_t sector = cluster_to_sector(current_cluster, boot_sector);

        // Read all sectors in the cluster
        for (uint8_t i = 0; i < boot_sector->sectors_per_cluster; i++) {
            if (read_sector(img_file, sector + i, buffer) != 0) {
                printf("Error reading directory sector!\n");
                return;
            }

            // Traverse the directory entries in the buffer
            for (size_t offset = 0; offset < 512; offset += sizeof(struct FAT32_DirectoryEntry)) {
                struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[offset];

                // Skip empty or unused entries
                if (entry->name[0] == 0x00 || entry->name[0] == 0xE5) {
                    continue;
                }

                // Check if the entry is a file or directory
                char type[8];
                if (entry->attr & 0x10) {
                    strcpy(type, "Directory");
                } else if (entry->attr & 0x20) {
                    strcpy(type, "File");
                } else {
                    strcpy(type, "Unknown");
                }

                // Format the filename (convert 8.3 format to standard string)
                char filename[13];
                memcpy(filename, entry->name, 8);
                filename[8] = '\0';

                // Trim trailing spaces in the filename
                for (int j = 7; j >= 0 && filename[j] == ' '; j--) {
                    filename[j] = '\0';
                }

                // Add extension if the entry is a file
                if (!(entry->attr & 0x10)) { // Not a directory
                    strcat(filename, ".");
                    strncat(filename, entry->name + 8, 3);
                }

                // Print the entry details
                uint32_t start_cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;
                printf("%-12s %-8s %-10u %-10u\n", filename, type, start_cluster, entry->file_size);
            }
        }

        // Move to the next cluster in the chain
        current_cluster = get_next_cluster(img_file, boot_sector, current_cluster);
    }
}



int main() {
    const char *image_path = "filesystem.img";  // Path to your FAT32 filesystem image
    FILE *img_file = fopen(image_path, "rb");
    if (!img_file) {
        perror("Error opening image file");
        return -1;
    }

    struct FAT32_BootSector boot_sector;
    uint8_t buffer[512];

    // Read the boot sector (sector 0)
    if (read_sector(img_file, 0, buffer) != 0) {
        printf("Error reading boot sector!\n");
        fclose(img_file);
        return -1;
    }

    // Copy boot sector information into our structure
    simple_memcpy(&boot_sector, buffer, sizeof(struct FAT32_BootSector));

    // Print out the boot sector information
    printf("FAT32 Boot Sector Information:\n");
    printf("OEM Name: %s\n", boot_sector.oem_name);
    printf("Bytes per sector: %d\n", boot_sector.bytes_per_sector);
    printf("Sectors per cluster: %d\n", boot_sector.sectors_per_cluster);
    printf("Reserved sectors: %d\n", boot_sector.reserved_sectors);
    printf("Number of FATs: %d\n", boot_sector.num_fats);
    printf("Sectors per FAT: %d\n", boot_sector.sectors_per_fat);
    // printf("Root Cluster: %u\n", ntohl(boot_sector.root_cluster));
    printf("Root Cluster: %u\n", boot_sector.root_cluster);
    printf("FSInfo sector: %u\n", boot_sector.fs_info);
    printf("Backup boot sector: %u\n", boot_sector.backup_boot_sector);

    // Now print the root directory entries
    print_root_directory(img_file, &boot_sector);

    // Close the image file
    fclose(img_file);
    return 0;
}
