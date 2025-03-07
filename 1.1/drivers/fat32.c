#include "fat32.h"
#include "disk_io.h"
#include "../libs/printf.h"


struct FAT32_BootSector boot_sector;

int fat32_start_sector;

uint32_t current_directory_cluster;
uint32_t root_directory_cluster;


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

        current_directory_cluster = boot_sector.root_cluster;
        root_directory_cluster = boot_sector.root_cluster;
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


int convert_to_int(int num_array[], int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        if (num_array[i] == -1) break;  // Stop if we hit an empty slot
        result = result * 10 + num_array[i];  // Convert digit array to int
    }
    return result;
}


uint32_t get_file_entry(const char *filename){
    // Starting sector of the root directory
    // uint32_t root_cluster = boot_sector.root_cluster;
    uint32_t root_cluster = current_directory_cluster;
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
                // return file_entry;
                return (file_entry->first_cluster_high << 16) | file_entry->first_cluster_low;
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

    return NULL;
}



void delete_entry(const char *filename) {
    uint32_t root_cluster = current_directory_cluster;
    uint32_t sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;

    uint8_t buffer[512];
    struct FAT32_DirectoryEntry *file_entry = NULL;
    int entry_offset = -1;

    while (root_cluster < 0x0FFFFFF8) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            if (entry->name[0] == 0x00) {
                return;
            }
            if (entry->name[0] == 0xE5) {
                continue;
            }
            if (entry->attributes & 0x0F) {
                continue;
            }

            char entry_name[12] = {0};
            for (int j = 0; j < 11; j++) {
                entry_name[j] = entry->name[j];
            }
            entry_name[11] = '\0';

            if (strncmp(entry_name, filename, 11) == 0) {
                file_entry = entry;
                entry_offset = i;
                break;
            }
        }

        if (file_entry != NULL) {
            break;
        }

        root_cluster = get_next_cluster(root_cluster);
        sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;
    }

    if (file_entry == NULL) {
        printf("File not found: %s\n", filename);
        return;
    }

    file_entry->name[0] = 0xE5;
    write_sector(sector_number, buffer);

    uint32_t file_cluster = (file_entry->first_cluster_high << 16) | file_entry->first_cluster_low;
    uint32_t next_cluster;

    // while (file_cluster < 0x0FFFFFF8) {
    //     next_cluster = get_next_cluster(file_cluster);
        // set_fat_entry(file_cluster, 0x00000000);
    //     file_cluster = next_cluster;
    // }

    // printf("File deleted successfully: %s\n", filename);
}




void read_file(const char *filename) {
    // Starting sector of the root directory
    // uint32_t root_cluster = boot_sector.root_cluster;
    uint32_t root_cluster = current_directory_cluster;
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
    
    // printf("Reading file: %s (Size: %u bytes)\n", filename, file_size);
    // printf("Starting Cluster: %x\n", file_cluster);

    // while (file_size > 0 && file_cluster < 0x0FFFFFF8) {
    while (file_size > 0) {
    // while (file_cluster < 0x0FFFFFF8){
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
        // sleep();
        // sleep();
    }

    printf("\nFile read completed.\n");
}

void draw_png_from_txt(const char *filename) {
    uint32_t root_cluster = current_directory_cluster;
    uint32_t sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;
    uint8_t buffer[512];
    struct FAT32_DirectoryEntry *file_entry = NULL;

    // Locate the file
    while (root_cluster < 0x0FFFFFF8) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }
        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];
            if (entry->name[0] == 0x00) return;
            if (entry->name[0] == 0xE5 || (entry->attributes & 0x0F)) continue;
            char entry_name[12] = {0};
            for (int j = 0; j < 11; j++) entry_name[j] = entry->name[j];
            entry_name[11] = '\0';
            if (strncmp(entry_name, filename, 11) == 0) {
                file_entry = entry;
                break;
            }
        }
        if (file_entry) break;
        root_cluster = get_next_cluster(root_cluster);
        sector_number = cluster_to_sector(root_cluster) + fat32_start_sector;
    }
    if (!file_entry) {
        printf("File not found: %s\n", filename);
        return;
    }

    uint32_t file_cluster = (file_entry->first_cluster_high << 16) | file_entry->first_cluster_low;
    uint32_t file_size = file_entry->file_size;
    uint32_t bytes_read = 0;

        while (file_size > 0) {
    // while (file_cluster < 0x0FFFFFF8){
        // Calculate the sector corresponding to the current cluster
        sector_number = cluster_to_sector(file_cluster) + fat32_start_sector;

        for (int i = 0; i < boot_sector.sectors_per_cluster; i++) {
            if (read_sector(sector_number + i, buffer) != 0) {
                printf("Error reading sector %u\n", sector_number + i);
                return;
            }

            uint32_t bytes_to_print = (file_size > 512) ? 512 : file_size;
            
            int vals[5] = {-1, -1, -1, -1, -1};  // Stores final x, y, r, g, b values
            int num_array[5] = {-1, -1, -1, -1, -1};  // Temporarily holds digits of a number
            int num_idx = 0;  // Tracks position in num_array
            int val_idx = 0;  // Tracks position in vals[]
            // Print the sector's contents
            for (uint32_t j = 0; j < bytes_to_print; j++) {
                // if(buffer[j] == '{' || buffer[j] == '}' || buffer[j] == ','){
                //     continue;
                // }
                // printf("%c", buffer[j]);
                // sleep();
                // sleep();

                char c = buffer[j];

                if (c >= '0' && c <= '9') {
                    if (num_idx < 5) {
                        num_array[num_idx++] = c - '0';  // Convert char to int and store
                    }
                } 
                else if (c == ',' || c == '}') {
                    if (num_idx > 0) {
                        vals[val_idx++] = convert_to_int(num_array, num_idx);
                        // Reset num_array for the next number
                        num_idx = 0;
                        for (int i = 0; i < 5; i++) num_array[i] = -1;
                    }
                }

                if (c == '}') {
                    // Print extracted values
                    // printf("Extracted values: x=%d, y=%d, r=%d, g=%d, b=%d\n", 
                    //     vals[0], vals[1], vals[2], vals[3], vals[4]);
                    
                    // sleep();
                    // sleep();

                    draw_pixel(vals[0], vals[1], vals[2], vals[3], vals[4]);
                    // Reset vals[] for the next pixel entry
                    for (int i = 0; i < 5; i++) vals[i] = -1;
                    val_idx = 0;
                }
                
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
        // sleep();
        // sleep();
    }


}




void ls() {
    uint32_t sector_number = cluster_to_sector(current_directory_cluster) + fat32_start_sector;
    uint8_t buffer[512];

    printf("Listing directory contents...\n");

    uint32_t directory_cluster = current_directory_cluster;
    while (directory_cluster < 0x0FFFFFF8) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            if (entry->name[0] == 0x00) {
                return; // End of directory
            }
            if (entry->name[0] == 0xE5) {
                continue; // Deleted entry
            }
            if (entry->attributes & 0x0F) {
                continue; // Skip long name entries
            }

            // Print name and type
            printf("Name: ");
            for (int j = 0; j < 11; j++) {
                if (entry->name[j] != ' ') {
                    printf("%c", entry->name[j]);
                }
            }

            if (entry->attributes & 0x10) {
                printf(" [DIR]");
            } else {
                printf(" [FILE]");
            }
            printf(" Cluster number: %d", (entry->first_cluster_high << 16) | entry->first_cluster_low);
            printf(" ATTRIBUTES: %d", entry->attributes);
            printf("\n");
        }

        // Move to the next cluster in the directory
        directory_cluster = get_next_cluster(directory_cluster);
        sector_number = cluster_to_sector(directory_cluster) + fat32_start_sector;
    }
}



void cd(const char *dirname) {
    uint32_t sector_number = cluster_to_sector(current_directory_cluster) + fat32_start_sector;
    uint8_t buffer[512];
    uint32_t directory_cluster = current_directory_cluster;

    while (directory_cluster < 0x0FFFFFF8) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            if (entry->name[0] == 0x00) {
                printf("Directory not found: %s\n", dirname);
                return;
            }
            if (entry->name[0] == 0xE5) {
                continue; // Deleted entry
            }
            if (entry->attributes & 0x0F) {
                continue; // Skip long name entries
            }

            // Match directory name
            char entry_name[12] = {0};
            for (int j = 0; j < 11; j++) {
                entry_name[j] = entry->name[j];
            }
            entry_name[11] = '\0';

            if (strncmp(entry_name, dirname, 11) == 0 && (entry->attributes & 0x10)) {
                current_directory_cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;
                printf("Changed directory to: %s\n", dirname);
                return;
            }
        }

        // Move to the next cluster in the directory
        directory_cluster = get_next_cluster(directory_cluster);
        sector_number = cluster_to_sector(directory_cluster) + fat32_start_sector;
    }

    printf("Directory not found: %d\n", sizeof(dirname));
}


void cd_up() {
    uint32_t sector_number = cluster_to_sector(current_directory_cluster) + fat32_start_sector;
    uint8_t buffer[512];
    uint32_t directory_cluster = current_directory_cluster;

    while (directory_cluster < 0x0FFFFFF8) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return;
        }

        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            if (entry->name[0] == 0x00) {
                printf("No parent directory found.\n");
                return; // End of directory, should never happen in valid FAT32
            }
            if (entry->name[0] == 0xE5) {
                continue; // Deleted entry
            }
            if (entry->attributes & 0x0F) {
                continue; // Skip long name entries
            }

            // Check for ".." entry
            if (strncmp((char *)entry->name, "..         ", 11) == 0) {
                // Extract the cluster of the parent directory
                uint32_t parent_cluster = (entry->first_cluster_high << 16) | entry->first_cluster_low;

                // Correctly handle the root directory case
                if (current_directory_cluster == root_directory_cluster) {
                    printf("Already at the root directory.\n");
                } else {
                    // printf("PARENT CLUSTER: %d\n", parent_cluster);
                    if(parent_cluster == 0){
                        current_directory_cluster = root_directory_cluster;
                    }
                    else{
                        current_directory_cluster = parent_cluster;
                    }
                    printf("Moved to parent directory.\n");
                }
                return;
            }
        }

        // Move to the next cluster in the directory
        directory_cluster = get_next_cluster(directory_cluster);
        sector_number = cluster_to_sector(directory_cluster) + fat32_start_sector;
    }

    printf("No parent directory found.\n");
}




uint32_t read_fat_entry(uint32_t cluster_number) {
    // Calculate byte offset of the FAT entry in the FAT table
    uint32_t fat_offset = cluster_number * 4;

    // Determine the sector containing this FAT entry
    uint32_t fat_sector = fat32_start_sector + (fat_offset / boot_sector.bytes_per_sector);

    // Offset within the sector
    uint16_t entry_offset = fat_offset % boot_sector.bytes_per_sector;

    uint8_t buffer[512];

    // Read the FAT sector
    if (read_sector(fat_sector, buffer) != 0) {
        printf("Error reading FAT sector: %u\n", fat_sector);
        return 0xFFFFFFFF; // Return error
    }

    // Extract the FAT entry (4 bytes)
    uint32_t fat_entry = *(uint32_t *)&buffer[entry_offset];

    // Mask the upper 4 bits (FAT32 uses only 28 bits)
    return fat_entry & 0x0FFFFFFF;
}


struct FAT32_DirectoryEntry* find_free_directory_entry(uint8_t* buffer) {
    for (int i = 0; i < 512; i += 32) {
        struct FAT32_DirectoryEntry* entry = (struct FAT32_DirectoryEntry*)&buffer[i];
        if (entry->name[0] == 0x00 || entry->name[0] == 0xE5) {
            return entry; // Found free entry
        }
    }
    return NULL; // No free entry found
}

uint32_t find_free_cluster() {
    int fat_size = (boot_sector.total_sectors_32 - boot_sector.reserved_sectors - (boot_sector.num_fats * boot_sector.sectors_per_fat)) / boot_sector.sectors_per_cluster;
    for (uint32_t i = 2; i < fat_size; i++) {
        if (read_fat_entry(i) == 0x00000000) {
            return i; // Found free cluster
        }
    }
    return 0xFFFFFFFF; // No free cluster
}


void write_fat_entry(uint32_t cluster, uint32_t value) {
    uint32_t fat_offset = cluster * 4;
    uint32_t sector = fat32_start_sector + (fat_offset / 512);
    uint32_t offset = fat_offset % 512;

    uint8_t buffer[512];
    read_sector(sector, buffer);
    *((uint32_t*)&buffer[offset]) = value;
    write_sector(sector, buffer);
}


int create_entry(const char* name, uint8_t attributes) {
    uint32_t sector_number = cluster_to_sector(current_directory_cluster) + fat32_start_sector;
    uint8_t buffer[512];

    

    while (1) {
        
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return -1;
        }

        struct FAT32_DirectoryEntry* entry = find_free_directory_entry(buffer);
        
        if (entry != NULL) {
               
            // Found a free entry, fill it
            memset(entry->name, ' ', 11);
            for (int i = 0; i < 11 && name[i] != '\0'; i++) {
                entry->name[i] = name[i];
            }
            
            entry->attributes = attributes;
        
            // Allocate a cluster for the file/directory
            uint32_t cluster = find_free_cluster();
             
            if (cluster == 0xFFFFFFFF) {
                printf("No free clusters available.\n");
                return -1;
            }
            write_fat_entry(cluster, 0x0FFFFFFF); // end of chain
            
            entry->first_cluster_low = cluster & 0xFFFF;
            entry->first_cluster_high = (cluster >> 16) & 0xFFFF;

            if (attributes & 0x10) {
                
                uint8_t dir_buffer[512] = {0};
                struct FAT32_DirectoryEntry* dot = (struct FAT32_DirectoryEntry*)&dir_buffer[0];
                struct FAT32_DirectoryEntry* dotdot = (struct FAT32_DirectoryEntry*)&dir_buffer[32];
                
                // "." entry
                memset(dot->name, ' ', 11);
                dot->name[0] = '.';
                dot->attributes = 0x10;
                dot->first_cluster_low = cluster & 0xFFFF;
                dot->first_cluster_high = (cluster >> 16) & 0xFFFF;
                
                // ".." entry
                memset(dotdot->name, ' ', 11);
                dotdot->name[0] = '.';
                dotdot->name[1] = '.';
                dotdot->attributes = 0x10;
                dotdot->first_cluster_low = current_directory_cluster & 0xFFFF;
                dotdot->first_cluster_high = (current_directory_cluster >> 16) & 0xFFFF;
                
                // printf("cluster: %d, cluster_to_sector: %d\n", cluster, cluster_to_sector(cluster)+ fat32_start_sector);
                write_sector(cluster_to_sector(cluster) + fat32_start_sector, dir_buffer);
                // printf("SHUSHHH");
            }
            
            write_sector(sector_number, buffer);
            printf("Created %s\n", (attributes & 0x10) ? "directory" : "file");
            return 0;
        }
        

        // Move to the next cluster
        uint32_t next_cluster = get_next_cluster(current_directory_cluster);
        if (next_cluster >= 0x0FFFFFF8) {
            printf("No space in directory to create entry.\n");
            return -1;
        }
        sector_number = cluster_to_sector(next_cluster) + fat32_start_sector;
    }
}



int update_file_size(const char *filename, uint32_t new_size) {
    uint32_t sector_number = cluster_to_sector(current_directory_cluster) + fat32_start_sector;
    uint8_t buffer[512];
    uint32_t directory_cluster = current_directory_cluster;

    printf(" yo1 + %d", directory_cluster);

    // while (directory_cluster < 0x0FFFFFF8) {
    while (1) {
        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading sector %u\n", sector_number);
            return -1;
        }

        printf("yo2");
        for (int i = 0; i < 512; i += 32) {
            struct FAT32_DirectoryEntry *entry = (struct FAT32_DirectoryEntry *)&buffer[i];

            if (entry->name[0] == 0x00) {
                return -1; // End of directory, file not found
            }
            if (entry->name[0] == 0xE5) {
                continue; // Deleted entry
            }
            if (entry->attributes & 0x0F) {
                continue; // Skip long name entries
            }

            // Match the file name
            char entry_name[12] = {0};
            for (int j = 0; j < 11; j++) {
                entry_name[j] = entry->name[j];
            }
            entry_name[11] = '\0';

            if (strncmp(entry_name, filename, 11) == 0) {
                // Update the file size
                entry->file_size = new_size;
                printf("yo3");
                // Write back the updated directory entry
                if (write_sector(sector_number, buffer) != 0) {
                    printf("Error writing updated directory entry\n");
                    return -1;
                }

                printf("yo4");
                // printf("Updated file size for %s to %u bytes\n", filename, new_size);
                return 0;
            }
        }

        // Move to the next cluster in the directory
        directory_cluster = get_next_cluster(directory_cluster);
        sector_number = cluster_to_sector(directory_cluster) + fat32_start_sector;
    }

    printf("File not found: %s\n", filename);
    return -1;
}





// int overwrite_file(uint32_t file_start_cluster, const uint8_t *data, uint32_t data_length, const char *filename) {
int overwrite_file(const uint8_t *data, uint32_t data_length, const char *filename) {
    
    ////
    uint32_t root_cluster = current_directory_cluster;
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
    
    
    ////
    // uint32_t current_cluster = file_start_cluster;
    uint32_t current_cluster = file_cluster;
    uint32_t bytes_written = 0;

   
    
    while (data_length > 0) {
        uint32_t sector_number = cluster_to_sector(current_cluster);
        uint32_t bytes_to_write = (data_length > 512) ? 512 : data_length;


        // Write the data to the current cluster
        if (write_sector(sector_number + fat32_start_sector, data + bytes_written) != 0) {
            printf("Error writing to sector %u\n", sector_number);
            return -1; // Error
        }
        

        bytes_written += bytes_to_write;
        data_length -= bytes_to_write;
        
        // Allocate a new cluster if needed
        if (data_length > 0) {
            uint32_t next_cluster = read_fat_entry(current_cluster);
            if (next_cluster >= 0x0FFFFFF8 || next_cluster == 0x00000000) {
                // Allocate a new cluster
                next_cluster = find_free_cluster();
                if (next_cluster == 0xFFFFFFFF) {
                    printf("No free clusters available\n");
                    return -1; // Error
                } 
                write_fat_entry(current_cluster, next_cluster);
                write_fat_entry(next_cluster, 0x0FFFFFF8); // Mark as end-of-chain
            }
            current_cluster = next_cluster;
        }
    }
    
    // printf("yo");
    
    // printf("current cluster: %d\n", current_cluster);
    // // Free any remaining clusters in the chain
    // uint32_t next_cluster = read_fat_entry(current_cluster);
    // printf("YOOOO SHUTTUPPP\n");
    // printf("nect cluster %d\n", next_cluster);
    // while (next_cluster < 0x0FFFFFF8) {
    // // while (next_cluster <= 0x0FFFFFFF) {
    //     uint32_t temp = next_cluster;
    //     printf("%d ", temp);
    //     next_cluster = read_fat_entry(next_cluster);
    //     write_fat_entry(temp, 0x00000000); // Mark as free
        
    // }


    // write_fat_entry(current_cluster, 0x0FFFFFF8); // Mark current cluster as end-of-chain

    

    // Update the file size
    if (update_file_size(filename, bytes_written) != 0) {
        printf("Failed to update file size\n");
        return -1;
    }
    printf("done");

    // return bytes_written;

    return bytes_written;
}





int append_to_file(uint32_t file_start_cluster, const uint8_t *data, uint32_t data_length, uint32_t file_size , const char *filename) {
    uint32_t current_cluster = file_start_cluster;
    uint32_t bytes_written = 0;

    // Traverse to the last cluster
    while (1) {
        uint32_t next_cluster = read_fat_entry(current_cluster);
        if (next_cluster >= 0x0FFFFFF8) {
            break; // End of chain
        }
        current_cluster = next_cluster;
    }

    // Calculate the offset within the last cluster
    uint32_t offset_in_last_cluster = file_size % 512;

    // Write data to the last cluster first if there's space
    if (offset_in_last_cluster > 0) {
        uint32_t sector_number = cluster_to_sector(current_cluster);
        uint8_t buffer[512];

        if (read_sector(sector_number, buffer) != 0) {
            printf("Error reading last sector of the file\n");
            return -1; // Error
        }

        uint32_t bytes_to_write = (512 - offset_in_last_cluster < data_length)
                                      ? (512 - offset_in_last_cluster)
                                      : data_length;

        simple_memcpy(buffer + offset_in_last_cluster, data, bytes_to_write);

        if (write_sector(sector_number, buffer) != 0) {
            printf("Error writing to last sector of the file\n");
            return -1; // Error
        }

        bytes_written += bytes_to_write;
        data_length -= bytes_to_write;
    }

    // Allocate new clusters for the remaining data
    while (data_length > 0) {
        uint32_t next_cluster = find_free_cluster();
        if (next_cluster == 0xFFFFFFFF) {
            printf("No free clusters available\n");
            return -1; // Error
        }
        write_fat_entry(current_cluster, next_cluster);
        write_fat_entry(next_cluster, 0x0FFFFFF8); // Mark as end-of-chain

        current_cluster = next_cluster;
        uint32_t sector_number = cluster_to_sector(current_cluster);
        uint32_t bytes_to_write = (data_length > 512) ? 512 : data_length;

        if (write_sector(sector_number, data + bytes_written) != 0) {
            printf("Error writing to sector %u\n", sector_number);
            return -1; // Error
        }

        bytes_written += bytes_to_write;
        data_length -= bytes_to_write;
    }

    uint32_t new_file_size = file_size + data_length;
    if (update_file_size(filename, new_file_size) != 0) {
        printf("Failed to update file size\n");
        return -1;
    }


    return bytes_written;
}
