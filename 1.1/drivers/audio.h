// audio.h
#ifndef AUDIO_H
#define AUDIO_H

#include "../libs/stdint.h"

// Function prototypes
void speaker_enable();
void speaker_disable();
void speaker_set_frequency(uint32_t frequency);
void speaker_beep(uint32_t frequency, uint32_t duration);

#endif // AUDIO_H
