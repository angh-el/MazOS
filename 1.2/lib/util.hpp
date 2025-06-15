#ifndef UTIL_HPP
#define UTIL_HPP


#include "stdint.h"

#define CEIL_DIV(a,b) (((a+b)-1)/b)

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }
};

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

struct interrupt_register{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;

};


#endif