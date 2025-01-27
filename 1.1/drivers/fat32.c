#include "fat32.h"
#include "disk_io.h"
#include "../libs/printf.h"


struct FAT32_BootSector boot_sector;

int fat32_start_sector;

// uint32_t get_next_cluster(uint32_t current_cluster) {
//     // uint32_t fat_offset =  current_cluster * 4 + 22712;  // 4 bytes per FAT entry
//     uint32_t fat_offset =  current_cluster * 4 + 0;
//     uint32_t fat_sector = 0 + boot_sector.reserved_sectors + (fat_offset / boot_sector.bytes_per_sector);
//     uint32_t fat_entry_offset = fat_offset % boot_sector.bytes_per_sector;

//     uint8_t buffer[boot_sector.bytes_per_sector];
//     read_sector(fat_sector, buffer);

//     for (int i = 0; i < 512; i++){
//         printf("%c", buffer[i]);
//     }
    
//     // Return the cluster number, masking out the upper bits
//     return *(uint32_t *)&buffer[fat_entry_offset] & 0x0FFFFFFF;
// }



uint32_t get_next_cluster(uint32_t current_cluster) {
    // Calculate the offset in the FAT for the current cluster
    uint32_t fat_offset = current_cluster * 4;  // FAT32 entries are 4 bytes
    uint32_t fat_sector = fat32_start_sector + boot_sector.reserved_sectors + (fat_offset / boot_sector.bytes_per_sector);
    uint32_t fat_entry_offset = fat_offset % boot_sector.bytes_per_sector;

    uint8_t buffer[boot_sector.bytes_per_sector];
    if (read_sector(fat_sector, buffer) != 0) {
        printf("Error reading FAT sector %u\n", fat_sector);
        return 0x0FFFFFF8;  // Return EOC (End of Chain) on error
    }

    // Read the 4-byte FAT entry
    uint32_t next_cluster = *(uint32_t *)&buffer[fat_entry_offset] & 0x0FFFFFFF;

    // Debugging output
    // printf("Current cluster: %u, Next cluster: %u\n", current_cluster, next_cluster);

    return next_cluster;
}


uint32_t cluster_to_sector(uint32_t cluster) {
    // return boot_sector.reserved_sectors + (boot_sector.num_fats * boot_sector.sectors_per_fat) + (cluster - 2) * boot_sector.sectors_per_cluster;
    // Calculate the first data sector
    uint32_t first_data_sector = boot_sector.reserved_sectors + (boot_sector.num_fats * boot_sector.sectors_per_fat);
    // printf("first_data_sector: %d\n", first_data_sector);
    // Calculate the sector for the given cluster (clusters start from 2)
    
    
    return first_data_sector + (cluster - 2) * boot_sector.sectors_per_cluster;
}




// Simple memory copy function
void simple_memcpy(void* dest, const void* src,  unsigned int n) {
    uint8_t* d = (uint8_t*) dest;
    const uint8_t* s = (const uint8_t*) src;
    for ( unsigned int i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

int strncmp(const char *str1, const char *str2, unsigned int n) {
    while (n--) {
        // Compare the characters at the current position
        if (*str1 != *str2) {
            return (*str1 - *str2);  // Return the difference between the two characters
        }

        // If we reached the end of either string, return 0
        if (*str1 == '\0') {
            return 0;
        }

        // Move to the next characters
        str1++;
        str2++;
    }
    return 0;  // If n characters are equal, return 0
}


int fat32_mount(uint32_t sector_number) {
    printf("Mounting Filesystem ...\n");
    for(int sector = 0; sector< get_disk_size(); sector++){

        // Step 1: Read the boot sector into the buffer
        uint8_t buffer[512];
        if (read_sector(sector, buffer) != 0) {
            // printf("Error reading boot sector!\n");
            // return -1;  // Return error if we fail to read the boot sector
        }

        // Step 2: Copy boot sector information into our structure
        simple_memcpy(&boot_sector, buffer, sizeof(struct FAT32_BootSector));

        // Step 3: Validate the boot sector (you can extend validation as per your needs)
        if (boot_sector.bytes_per_sector == 0 || boot_sector.sectors_per_cluster == 0) {
            // printf("Invalid boot sector data!\n");
            // return -1;
        }

        if(strncmp(boot_sector.oem_name, "mkfs.fat", 8) == 0){
        // if(strncmp(boot_sector.oem_name, "Wagwan", 6) == 0){
            // Step 4: Print out the boot sector information to ensure correctness
            fat32_start_sector = sector;
            printf("FAT32 Boot Sector Information (found at sector %d):\n", sector);
            printf("OEM Name: %s\n", boot_sector.oem_name);
            printf("Bytes per sector: %d\n", boot_sector.bytes_per_sector);
            printf("Sectors per cluster: %d\n", boot_sector.sectors_per_cluster);
            printf("Reserved sectors: %d\n", boot_sector.reserved_sectors);
            printf("Number of FATs: %d\n", boot_sector.num_fats);
            printf("Sectors per FAT: %d\n", boot_sector.sectors_per_fat);
            printf("Root Cluster: %u\n", boot_sector.root_cluster);
            
            printf("\n");
            return 0;
        }


        // Step 5: Return success (this will allow further operations like reading the root directory)
        // return 0;
    }
    
    
}


// Function to read and print the root directory contents
void read_root_directory() {
    // Calculate the starting sector of the root directory
    uint32_t root_cluster = boot_sector.root_cluster;
    uint32_t sector_number = cluster_to_sector(root_cluster) + fat32_start_sector; // Adjust for GRUB-loaded offset

    uint8_t buffer[512]; // Buffer to hold sector data

    printf("Reading root directory...\n");

    // Iterate through clusters in the root directory
    while (root_cluster < 0x0FFFFFF8) { // FAT32 end-of-cluster marker is 0x0FFFFFF8 or higher
        // Read the sector corresponding to the current cluster
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        // Parse directory entries (16 entries per 512-byte sector, 32 bytes each)
        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            // Check if the entry is valid
            if (entry->name[0] == 0x00) {
                // End of directory
                return;
            }
            if (entry->name[0] == 0xE5) {
                // Deleted entry, skip it
                continue;
            }

            // Check attributes to distinguish files and directories
            if (entry->attributes & 0x0F) {
                // Skip long file name entries
                continue;
            }

            // Print the file or directory name
            printf("Name: ");
            for (int j = 0; j < 11; j++) {
                if (entry->name[j] != ' ') {
                    printf("%c", entry->name[j]);
                }
            }

            // Print if it's a directory or file
            if (entry->attributes & 0x10) {
                printf(" [DIR]");
            } else {
                printf(" [FILE]");
            }

            // Print file size
            printf(" Size: %u bytes", entry->file_size);

            // Print Starting Cluster
            uint32_t starting_cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;
            printf(" Starting Cluster: %u\n", starting_cluster);
        }

        // Move to the next cluster in the FAT
        root_cluster = get_next_cluster(root_cluster);
        sector_number = cluster_to_sector(root_cluster) + fat32_start_sector; // Adjust for GRUB-loaded offset
    }
}




// // Function to read and print the contents of a file
// void read_file(const char *filename) {
    
//     // Calculate the starting sector of the root directory
//     uint32_t root_cluster = boot_sector.root_cluster;
//     uint32_t sector_number = cluster_to_sector(root_cluster) + fat32_start_sector; // Adjust for GRUB-loaded offset

//     uint8_t buffer[512]; // Buffer to hold sector data
//     struct FAT32_DirectoryEntry *file_entry = NULL;

//     // Search for the file in the root directory
//     while (root_cluster < 0x0FFFFFF8) { // FAT32 end-of-cluster marker is 0x0FFFFFF8 or higher
        
//         // Read the sector corresponding to the current cluster
//         if (read_sector(sector_number, buffer) != 0) {
//             printf("Error reading sector %u\n", sector_number);
//             return;
//         }

        

//         // Parse directory entries (16 entries per 512-byte sector, 32 bytes each)
//         for (int i = 0; i < 512; i += 32) {
//             struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];
//             // printf("Reading File ...\n");
//             // Check if the entry is valid
//             if (entry->name[0] == 0x00) {
//                 // End of directory
//                 return;
//                 // continue;
//             }
//             if (entry->name[0] == 0xE5) {
//                 // Deleted entry, skip it
//                 continue;
//             }
//             if (entry->attributes & 0x0F) {
//                 // Skip long file name entries
//                 continue;
//             }

//             // Check if the entry matches the filename
//             char entry_name[12] = {0}; // To hold the formatted filename
//             for (int j = 0; j < 11; j++) {
//                 if (entry->name[j] != ' ') {
//                     entry_name[j] = entry->name[j];
//                 }
//             }
//             entry_name[11] = '\0'; // Null-terminate the string

//             if (strncmp(entry_name, filename, 11) == 0) {
//                 file_entry = entry; // Found the file
//                 break;
//             }
//         }

//         if (file_entry != NULL) {
//             // printf("NULL FILE ENTRY APPARENTLY WTF\n");
//             break; // Exit the loop if the file is found
//         }

//         // Move to the next cluster in the FAT
//         root_cluster = get_next_cluster(root_cluster);
//         // sector_number = cluster_to_sector(root_cluster) + fat32_start_sector; // Adjust for GRUB-loaded offset
//         sector_number = cluster_to_sector(root_cluster) ;
//     }
//     // printf("Reading File ...\n");

//     if (file_entry == NULL) {
//         printf("File not found: %s\n", filename);
//         return;
//     }

//     // Read the file's contents
//     uint32_t file_cluster = (file_entry->first_cluster_high << 16) | file_entry->first_cluster_low;
//     uint32_t file_size = file_entry->file_size;

//     printf("Reading file: %s (Size: %u bytes)\n", filename, file_size);
//     printf("First Cluster High: %x\n", file_entry->first_cluster_high);
//     printf("First Cluster Low: %x\n", file_entry->first_cluster_low);
//     printf("Cluster: %x\n", file_cluster);

//     while (file_size > 0 && file_cluster < 0x0FFFFFF8) {
        

//         // Calculate the sector corresponding to the current cluster
//         sector_number = cluster_to_sector(file_cluster) + fat32_start_sector;
        
//         // Read the cluster's data
//         for (int i = 0; i < boot_sector.sectors_per_cluster; i++) {
//             printf("Reading sector: %u\n", sector_number + i);  // Debugging output
//             if (read_sector(sector_number + i, buffer) != 0) {
//                 printf("yoo ");
//                 printf("Error reading sector %u\n", sector_number + i);
//                 return;
//             }
            
//             // Calculate the number of bytes to print from the current sector
//             uint32_t bytes_to_print = (file_size > 512) ? 512 : file_size;

//             // Print the sector's contents
//             for (uint32_t j = 0; j < bytes_to_print; j++) {
//                 // printf("i");
//                 printf("%c", buffer[j]);
//             }

//             // Decrease the remaining file size
//             file_size -= bytes_to_print;
//         }

        
//         // Move to the next cluster
//         file_cluster = get_next_cluster(file_cluster);
//     }

//     printf("\nFile read completed.\n");
// }





void read_file(const char *filename) {
    // Starting sector of the root directory
    uint32_t root_cluster = boot_sector.root_cluster;
    uint32_t sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;

    uint8_t buffer[512]; // Buffer to hold sector data
    struct FAT32_DirectoryEntry *file_entry = NULL;

    // Search for the file in the root directory
    while (root_cluster < 0x0FFFFFF8) { // FAT32 end-of-cluster marker
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        // Parse directory entries (16 entries per 512-byte sector, 32 bytes each)
        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            // Check if the entry is valid
            // printf("entry name: %s", entry->name);
            if (entry->name[0] == 0x00) {
                // End of directory
                // printf("yoooo");
                return;
            }
            if (entry->name[0] == 0xE5) {
                // Deleted entry, skip
                continue;
            }
            if (entry->attributes & 0x0F) {
                // Skip long file name entries
                continue;
            }

            // Match the filename
            char entry_name[12] = {0};
            for (int j = 0; j < 11; j++) {
                // if (entry->name[j] != ' ') {
                    entry_name[j] = entry->name[j];
                // }
            }
            // printf("entry name : %s", entry_name);
            entry_name[11] = '\0';

            if (strncmp(entry_name, filename, 11) == 0) {
                file_entry = entry; // Found the file
                break;
            }
        }

        if (file_entry != NULL) {
            break;
        }

        // Move to the next cluster in the root directory
        root_cluster = get_next_cluster(root_cluster);
        sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;
    }

    if (file_entry == NULL) {
        printf("File not found: %s\n", filename);
        return;
    }

    // Read the file's contents
    uint32_t file_cluster = (file_entry->first_cluster_high << 16) | file_entry->first_cluster_low;
    uint32_t file_size = file_entry->file_size;
    
    printf("Reading file: %s (Size: %u bytes)\n", filename, file_size);
    printf("Starting Cluster: %x\n", file_cluster);

    // while (file_size > 0 && file_cluster < 0x0FFFFFF8) {
    while (file_size > 0) {
        // Calculate the sector corresponding to the current cluster
        sector_number = cluster_to_sector(file_cluster) + fat32_start_sector;

        for (int i = 0; i < boot_sector.sectors_per_cluster; i++) {
            if (read_sector(sector_number + i, buffer) != 0) {
                printf("Error reading sector %u\n", sector_number + i);
                return;
            }

            uint32_t bytes_to_print = (file_size > 512) ? 512 : file_size;

            // Print the sector's contents
            for (uint32_t j = 0; j < bytes_to_print; j++) {
                printf("%c", buffer[j]);
            }

            file_size -= bytes_to_print;
            // printf(" file size: %d\n", file_size);
            if (file_size == 0) {
                break; // File completely read
            }
        }

        // Move to the next cluster in the FAT
        file_cluster = get_next_cluster(file_cluster);
        // printf("cluster : %d\n", file_cluster);
        if (file_cluster >= 0x0FFFFFF8) {
            break; // End of file reached
        }
    }

    printf("\nFile read completed.\n");
}



