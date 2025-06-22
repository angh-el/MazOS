#ifndef TIMER_HPP
#define TIMER_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../descriptors/idt.hpp"

class Timer {
public:
    static void init();
    static void handler(struct interrupt_register *regs);
    static void terminate();
    static int get_ticks();
    static void set_freq(uint32_t frequency);

};

void wrapper(struct interrupt_register *regs);

#endif