#pragma once

#include "stdint.h"

#define CEIL_DIV(a,b) (((a+b)-1)/b)

void memset(void *dest, char val, uint32_t count);


unsigned char port_byte_in ( unsigned short port );
// char port_byte_in ( uint16_t port );

void port_byte_out ( unsigned short port , unsigned char data );

unsigned short port_word_in ( unsigned short port );

void port_word_out ( unsigned short port , unsigned short data );

// struct interrupt_register{
//     uint32_t cr2;
//     uint32_t ds;
//     uint32_t edi;
//     uint32_t esi;
//     uint32_t ebp;
//     uint32_t esp;
//     uint32_t ebx;
//     uint32_t edx;
//     uint32_t ecx;
//     uint32_t eax;
//     uint32_t interrupt_num;
//     uint32_t error_code;
//     uint32_t eip;
//     uint32_t csm;
//     uint32_t eflags;
//     uint32_t useresp;
//     uint32_t ss;
// };

struct interrupt_register{
    uint32_t cr2;
    uint32_t ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, csm, eflags, useresp, ss;

};