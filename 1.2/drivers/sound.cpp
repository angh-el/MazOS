#include "sound.hpp"

#define DSP_RESET_PORT       0x226
#define DSP_READ_PORT        0x22A
#define DSP_WRITE_PORT       0x22C
#define DSP_STATUS_PORT      0x22E
#define DSP_READ_BUFFER      0x22A


void Sound::init() {
    port_byte_out(DSP_RESET_PORT, 1);
    for (volatile int i = 0; i < 100000; ++i); // delay
    port_byte_out(DSP_RESET_PORT, 0);

    uint8_t res = dsp_read();
    // printf("DSP reset response: 0x%x\n", res);

    if (dsp_read() != 0xAA) {
        printf("Sound driver initialisation failed\n");
        return;
    }
    printf("Succesfully initialised sound driver\n");
};

void Sound::play_note(char note, uint32_t duration_ms) {
    switch (note) {
        case 'C': play_freq(261, duration_ms); break;
        case 'D': play_freq(294, duration_ms); break;
        case 'E': play_freq(329, duration_ms); break;
        case 'F': play_freq(349, duration_ms); break;
        case 'G': play_freq(392, duration_ms); break;
        case 'A': play_freq(440, duration_ms); break;
        case 'B': play_freq(493, duration_ms); break;
        default: break;
    }
}

void Sound::play_freq(uint32_t freq, uint32_t duration_ms) {
    uint32_t divisor = 1193180 / freq;
    port_byte_out(0x43, 0xB6); // binary, mode 3, LSB/MSB, ch. 2
    port_byte_out(0x42, divisor & 0xFF);
    port_byte_out(0x42, (divisor >> 8) & 0xFF);

    uint8_t tmp = port_byte_in(0x61);
    port_byte_out(0x61, tmp | 3); // enable speaker

    for (volatile uint32_t i = 0; i < duration_ms * 1000; ++i);

    port_byte_out(0x61, tmp & 0xFC); // disable speaker
}

void Sound::wait_write() {
    while ((port_byte_in(DSP_STATUS_PORT) & 0x80) != 0);
}

void Sound::wait_read() {
    while ((port_byte_in(DSP_STATUS_PORT) & 0x80) == 0);
}

void Sound::dsp_write(uint8_t val) {
    wait_write();
    port_byte_out(DSP_WRITE_PORT, val);
}

uint8_t Sound::dsp_read() {
    wait_read();
    return port_byte_in(DSP_READ_PORT);
}