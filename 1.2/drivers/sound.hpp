#ifndef SOUND_HPP
#define SOUND_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"
#include "../lib/util.hpp"

class Sound {
public:
    static void init();
    static void play_note(char note, uint32_t duration_ms = 250);
    static void play_freq(uint32_t frequency, uint32_t duration_ms);

private:
    static void dsp_write(uint8_t val);
    static uint8_t dsp_read();
    static void wait_write();
    static void wait_read();

};

#endif