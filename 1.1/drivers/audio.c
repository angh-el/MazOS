// audio.c
#include "audio.h"
#include "../libs/util.h" 
// Define the PC speaker I/O ports
#define SPEAKER_CONTROL_PORT 0x61
#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT 0x42

// Enables the PC speaker by setting bits 0 and 1 of the control port
void speaker_enable() {
    port_byte_out(SPEAKER_CONTROL_PORT, port_byte_in(SPEAKER_CONTROL_PORT) | 0x03);
}

// Disables the PC speaker by clearing bits 0 and 1 of the control port
void speaker_disable() {
    port_byte_out(SPEAKER_CONTROL_PORT, port_byte_in(SPEAKER_CONTROL_PORT) & ~0x03);
}

// Sets the frequency of the speaker (timer frequency divisor)
void speaker_set_frequency(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;  // PIT timer frequency is 1.193180 MHz

    // Send the frequency to the PIT to set the speaker frequency
    port_byte_out(PIT_COMMAND_PORT, 0xB6);  // Command to set PIT channel 2 for square wave mode
    port_byte_out(PIT_DATA_PORT, (uint8_t)(divisor & 0xFF));  // Lower byte of divisor
    port_byte_out(PIT_DATA_PORT, (uint8_t)((divisor >> 8) & 0xFF));  // Upper byte of divisor
}

// Generates a beep of a specified frequency for a specified duration
void speaker_beep(uint32_t frequency, uint32_t duration) {
    speaker_enable();  // Enable the speaker
    speaker_set_frequency(frequency);  // Set the desired frequency (e.g., 440Hz for "A")
    
    // Simple delay loop (can be replaced with timer-based delay for more accuracy)
    for (volatile uint32_t i = 0; i < duration; i++) {
        // Loop for delay, or use your own timer function for better precision
    }

    speaker_disable();  // Disable the speaker after the beep
}
