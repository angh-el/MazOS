#include "timer.hpp"

uint64_t ticks;
uint32_t freq = 100;

void Timer::handler(struct interrupt_register *regs){
    ++ ticks;
}

void Timer::init(){
    ticks = 0;
    DescriptorTables::IDT::irq_install_handler(0, &wrapper);

    //119318.16666 Mhz
    uint32_t divisor = 1193180/freq;
    //0011 0110
    port_byte_out(0x43,0x36);
    port_byte_out(0x40,(uint8_t)(divisor & 0xFF));
    port_byte_out(0x40,(uint8_t)((divisor >> 8) & 0xFF));

    printf("Timer succesfully initialised\n");
}

void Timer::terminate(){
    printf("Timer succesfully terminated\n");
    DescriptorTables::IDT::clear_irq_handler(0);
}

int Timer::get_ticks(){
    return ticks;
}

void Timer::set_freq(uint32_t frequency){
    freq = frequency;
}

void wrapper(struct interrupt_register *regs){
    Timer::handler(regs);
}