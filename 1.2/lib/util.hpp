#ifndef UTIL_HPP
#define UTIL_HPP


#include "stdint.h"

#define CEIL_DIV(a,b) (((a+b)-1)/b)

void memset(void *dest, char val, uint32_t count);
// void memcpy(void *dest, const uint8_t * val, uint32_t count);

// void* memcpy(uint8_t* dest, const char* src, int len);
void* memcpy(uint8_t* dest, uint8_t* src, int len);

int strlen(const char* string);

inline unsigned char port_byte_in ( unsigned short port ){
    unsigned char result ;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result ;
};

inline void port_byte_out ( unsigned short port , unsigned char data ){
    __asm__("out %%al, %%dx" : : "a" (data), "d" ( port));
};

inline unsigned short port_word_in ( unsigned short port ){
    unsigned short result ;
    __asm__ ("in %%dx, %%ax" : "=a " ( result ) : "d" ( port ));
    return result ;
};

inline void port_word_out ( unsigned short port , unsigned short data ){
    __asm__ ("out %%ax, %%dx" : :"a" ( data ), "d" ( port ));
};

int strncmp(const char *str1, const char *str2, unsigned int n);
void simple_memcpy(void* dest, const void* src,  unsigned int n);
int convert_to_int(int num_array[], int size);


struct interrupt_register{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;

};


#endif