#include "util.h"

void memset(void *dest, char val, uint32_t count){
    char *temp = (char*) dest;
    for (; count != 0; count --){
        *temp++ = val;
    }
}

unsigned char port_byte_in ( unsigned short port ) {
    // A handy C wrapper function that reads a byte from the specified port
    // "=a" ( result ) means : put AL register in variable RESULT when finished
    // "d" ( port ) means : load EDX with port
    unsigned char result ;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result ;
}

// char port_byte_in ( uint16_t port ) {
//     // A handy C wrapper function that reads a byte from the specified port
//     // "=a" ( result ) means : put AL register in variable RESULT when finished
//     // "d" ( port ) means : load EDX with port
//     char result ;
//     asm volatile("inb %1, %0" : "=a" (result) : "d" (port));
//     return result ;
// }


void port_byte_out ( unsigned short port , unsigned char data ) {
    // "a" ( data ) means : load EAX with data
    // "d" ( port ) means : load EDX with port
    __asm__("out %%al, %%dx" : : "a" (data), "d" ( port));
}

unsigned short port_word_in ( unsigned short port ) {
    unsigned short result ;
    __asm__ ("in %%dx, %%ax" : "=a " ( result ) : "d" ( port ));
    return result ;
}

void port_word_out ( unsigned short port , unsigned short data ) {
    __asm__ ("out %%ax, %%dx" : :"a" ( data ), "d" ( port ));
}