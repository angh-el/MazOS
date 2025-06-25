#include "util.hpp"


void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }
};

// void memcpy(void *dest, const uint8_t * val, uint32_t count){
//     char *temp = (char*) dest;
//     for (; count != 0; count --){
//         *temp++ = *val;
//     }
// }


void* memcpy(uint8_t* dest, uint8_t* src, int len) {
    for (int i = 0; i < len; ++i) {
        dest[i] = static_cast<uint8_t>(src[i]);
    }
    return dest;
}

int strlen(const char* string) {
    int length = 0;
    while (string[length] != '\0') {
        ++length;
    }
    return length;
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

int convert_to_int(int num_array[], int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        if (num_array[i] == -1) break;  // Stop if we hit an empty slot
        result = result * 10 + num_array[i];  // Convert digit array to int
    }
    return result;
}