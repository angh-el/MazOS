#include "libs/printf.h"
#include "libs/util.h"
#include "idt.h"
#include "drivers/screen.h"
#include "timer.h"

uint64_t ticks;
uint32_t freq = 100;

void onIrq0(struct interrupt_register *regs){
    ticks += 1;
    // printf("%d ", ticks);
}

void init_timer(){
    ticks = 0;
    irq_install_handler(0,&onIrq0);

    //119318.16666 Mhz
    uint32_t divisor = 1193180/freq;

    //0011 0110
    port_byte_out(0x43,0x36);
    port_byte_out(0x40,(uint8_t)(divisor & 0xFF));
    port_byte_out(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}

void kill_timer(){
    clear_irq_handler(0);
}


int getTicks(){
    return ticks;
}

void setFreq(uint32_t frequency){
    freq = frequency;
}