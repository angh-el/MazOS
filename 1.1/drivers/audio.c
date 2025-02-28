#include "../libs/stdint.h"
#include "../libs/util.h"

// #define SB16_BASE_PORT  0x220
// #define DSP_RESET       (SB16_BASE_PORT + 0x06)
// #define DSP_READ        (SB16_BASE_PORT + 0x0A)


// int detect_sb16() {
//     port_byte_out(DSP_RESET, 1);  // Reset DSP
//     for (volatile int i = 0; i < 10000000; i++); // Small delay
//     port_byte_out(DSP_RESET, 0);  

//     // Check if DSP responds with 0xAA
//     return (port_byte_in(DSP_READ) == 0xAA);
// }



// #define DSP_WRITE (SB16_BASE_PORT + 0x0C)

// void sb16_set_sample_rate(uint16_t rate) {
//     port_byte_out(DSP_WRITE, 0x41);      // Command: Set sample rate
//     port_byte_out(DSP_WRITE, rate >> 8); // High byte
//     port_byte_out(DSP_WRITE, rate & 0xFF); // Low byte
// }


// #define DSP_PLAY_8BIT_MONO  0xC0

// void sb16_play_sound(uint8_t* buffer, uint16_t length) {
//     port_byte_out(DSP_WRITE, DSP_PLAY_8BIT_MONO);  // 8-bit mono mode
//     port_byte_out(DSP_WRITE, (length - 1) & 0xFF); // Length low byte
//     port_byte_out(DSP_WRITE, (length - 1) >> 8);   // Length high byte

//     // Now send sound data (simplified, actual DMA setup required)
//     for (int i = 0; i < length; i++) port_byte_out(SB16_BASE_PORT + 0x0C, buffer[i]); // Send PCM data
// }


// uint8_t sound_data[] = {127, 128, 129, 130, 128, 127, 126, 125}; // Simple waveform

// void play_woof() {
//     sb16_set_sample_rate(8000);
//     sb16_play_sound(sound_data, sizeof(sound_data));
// }

// //////////////////////////////////////////////////////////////////////////



/* Sound Blaster 16 Driver
 * For use with custom OS development
 * Uses port_byte_in and port_byte_out for I/O operations
 */

// #include <stdint.h>
// #include <stdbool.h>

/* Port Definitions */
#define SB16_MIXER_PORT       0x224
#define SB16_MIXER_DATA_PORT  0x225
#define SB16_DSP_RESET        0x226
#define SB16_DSP_READ         0x22A
#define SB16_DSP_WRITE        0x22C
#define SB16_DSP_READ_STATUS  0x22E
#define SB16_DSP_INT_ACK      0x22F

/* DMA Ports */
#define DMA_MASK_REG_8BIT     0x0A
#define DMA_FLIP_FLOP_8BIT    0x0C
#define DMA_MODE_REG_8BIT     0x0B
#define DMA_PAGE_CHAN1        0x83
#define DMA_ADDR_CHAN1        0x02
#define DMA_COUNT_CHAN1       0x03

#define DMA_MASK_REG_16BIT    0xD4
#define DMA_FLIP_FLOP_16BIT   0xD8
#define DMA_MODE_REG_16BIT    0xD6
#define DMA_PAGE_CHAN5        0x8B
#define DMA_ADDR_CHAN5        0xC4
#define DMA_COUNT_CHAN5       0xC6

/* DSP Commands */
#define DSP_CMD_SET_TIME_CONSTANT   0x40
#define DSP_CMD_SET_SAMPLE_RATE     0x41
#define DSP_CMD_SPEAKER_ON          0xD1
#define DSP_CMD_SPEAKER_OFF         0xD3
#define DSP_CMD_STOP_8BIT           0xD0
#define DSP_CMD_RESUME_8BIT         0xD4
#define DSP_CMD_STOP_16BIT          0xD5
#define DSP_CMD_RESUME_16BIT        0xD6
#define DSP_CMD_GET_VERSION         0xE1
#define DSP_CMD_PLAY_8BIT           0xC0
#define DSP_CMD_PLAY_16BIT          0xB0

/* Mixer Commands */
#define MIXER_CMD_MASTER_VOLUME     0x22
#define MIXER_CMD_SET_IRQ           0x80

/* IRQ Values for Mixer */
#define IRQ_2_VALUE                 0x01
#define IRQ_5_VALUE                 0x02
#define IRQ_7_VALUE                 0x04
#define IRQ_10_VALUE                0x08

/* DMA Mode Values */
#define DMA_MODE_SINGLE_8BIT        0x48
#define DMA_MODE_AUTO_8BIT          0x58
#define DMA_MODE_SINGLE_16BIT       0x48
#define DMA_MODE_AUTO_16BIT         0x58

/* Sound Format Types */
#define SOUND_FORMAT_MONO_UNSIGNED  0x00
#define SOUND_FORMAT_MONO_SIGNED    0x10
#define SOUND_FORMAT_STEREO_UNSIGNED 0x20
#define SOUND_FORMAT_STEREO_SIGNED  0x30

/* Basic structure to hold our SB16 information */
typedef struct {
    uint16_t base_port;     /* Base I/O port (usually 0x220) */
    uint8_t irq;            /* IRQ number (usually 5) */
    uint8_t dma_8bit;       /* 8-bit DMA channel (usually 1) */
    uint8_t dma_16bit;      /* 16-bit DMA channel (usually 5) */
    uint8_t major_version;  /* DSP Major version */
    uint8_t minor_version;  /* DSP Minor version */
} sb16_t;

/* Global SB16 instance */
static sb16_t sb16 = {
    .base_port = 0x220,
    .irq = 5,
    .dma_8bit = 1,
    .dma_16bit = 5,
    .major_version = 0,
    .minor_version = 0
};

/* Function declarations */
boolean sb16_reset();
boolean sb16_detect();
void sb16_set_irq(uint8_t irq);
void sb16_set_master_volume(uint8_t left, uint8_t right);
void sb16_speaker_on();
void sb16_speaker_off();
uint16_t sb16_get_version();
void sb16_program_dma_8bit(void* buffer, uint16_t length, boolean auto_init);
void sb16_program_dma_16bit(void* buffer, uint16_t length, boolean auto_init);
void sb16_play_sound_8bit(uint16_t sample_rate, void* buffer, uint16_t length, boolean stereo, boolean signed_data, boolean auto_init);
void sb16_play_sound_16bit(uint16_t sample_rate, void* buffer, uint16_t length, boolean stereo, boolean signed_data, boolean auto_init);
void sb16_stop_sound_8bit();
void sb16_stop_sound_16bit();
void sb16_acknowledge_interrupt_8bit();
void sb16_acknowledge_interrupt_16bit();

/**
 * Helper function to convert virtual address to physical
 * This is a placeholder - you'll need to implement this based on your OS's memory management
 */
uint32_t virtual_to_physical(void* address) {
    // This is where you would convert a virtual address to physical
    // for the purpose of programming the DMA controller
    // For now, just return the address as is (assuming identity mapping)
    return (uint32_t)address;
}

/**
 * Wait for the DSP to be ready to receive a command
 */
boolean sb16_dsp_ready_for_write() {
    uint8_t status;
    int timeout = 1000; // Arbitrary timeout

    while (timeout--) {
        status = port_byte_in(SB16_DSP_WRITE);
        if ((status & 0x80) == 0) {
            // Ready to accept data
            return true;
        }
        // Small delay would be good here
    }
    
    return false;
}

/**
 * Wait for the DSP to have data available to read
 */
boolean sb16_dsp_ready_for_read() {
    uint8_t status;
    int timeout = 1000; // Arbitrary timeout

    while (timeout--) {
        status = port_byte_in(SB16_DSP_READ_STATUS);
        if (status & 0x80) {
            // Data available
            return true;
        }
        // Small delay would be good here
    }
    
    return false;
}

/**
 * Write a command to the DSP
 */
void sb16_write_dsp(uint8_t value) {
    if (sb16_dsp_ready_for_write()) {
        port_byte_out(SB16_DSP_WRITE, value);
    } else {
        // Handle timeout error
    }
}

/**
 * Read data from the DSP
 */
uint8_t sb16_read_dsp() {
    if (sb16_dsp_ready_for_read()) {
        return port_byte_in(SB16_DSP_READ);
    } else {
        // Handle timeout error
        return 0;
    }
}

/**
 * Write a command to the mixer
 */
void sb16_write_mixer(uint8_t reg, uint8_t data) {
    port_byte_out(SB16_MIXER_PORT, reg);
    port_byte_out(SB16_MIXER_DATA_PORT, data);
}

/**
 * Read data from the mixer
 */
uint8_t sb16_read_mixer(uint8_t reg) {
    port_byte_out(SB16_MIXER_PORT, reg);
    return port_byte_in(SB16_MIXER_DATA_PORT);
}

/**
 * Reset the DSP
 */
boolean sb16_reset() {
    // Send 1 to DSP reset port
    port_byte_out(SB16_DSP_RESET, 1);
    
    // Wait at least 3 microseconds
    // You'll need to implement a delay function for your OS
    // For now, let's just create a simple busy wait
    for (volatile int i = 0; i < 100; i++);
    
    // Send 0 to DSP reset port
    port_byte_out(SB16_DSP_RESET, 0);
    
    // Read DSP data port, should return 0xAA
    if (sb16_dsp_ready_for_read()) {
        uint8_t response = port_byte_in(SB16_DSP_READ);
        return (response == 0xAA);
    }
    
    return false;
}

/**
 * Detect the Sound Blaster 16
 */
boolean sb16_detect() {
    // Try to reset the DSP
    if (!sb16_reset()) {
        return false;
    }
    
    // Get the DSP version
    uint16_t version = sb16_get_version();
    sb16.major_version = (version >> 8) & 0xFF;
    sb16.minor_version = version & 0xFF;
    
    // Check if version is 4.xx (Sound Blaster 16)
    return (sb16.major_version >= 4);
}

/**
 * Get the DSP version
 */
uint16_t sb16_get_version() {
    sb16_write_dsp(DSP_CMD_GET_VERSION);
    uint8_t major = sb16_read_dsp();
    uint8_t minor = sb16_read_dsp();
    return ((major << 8) | minor);
}

/**
 * Set the IRQ through the mixer
 */
void sb16_set_irq(uint8_t irq) {
    uint8_t irq_value = 0;
    
    switch (irq) {
        case 2:
            irq_value = IRQ_2_VALUE;
            break;
        case 5:
            irq_value = IRQ_5_VALUE;
            break;
        case 7:
            irq_value = IRQ_7_VALUE;
            break;
        case 10:
            irq_value = IRQ_10_VALUE;
            break;
        default:
            return;  // Invalid IRQ
    }
    
    sb16_write_mixer(MIXER_CMD_SET_IRQ, irq_value);
    sb16.irq = irq;
}

/**
 * Set the master volume
 */
void sb16_set_master_volume(uint8_t left, uint8_t right) {
    // Each channel has a range of 0-15
    if (left > 15) left = 15;
    if (right > 15) right = 15;
    
    uint8_t volume = (left << 4) | right;
    sb16_write_mixer(MIXER_CMD_MASTER_VOLUME, volume);
}

/**
 * Turn the speaker on
 */
void sb16_speaker_on() {
    sb16_write_dsp(DSP_CMD_SPEAKER_ON);
}

/**
 * Turn the speaker off
 */
void sb16_speaker_off() {
    sb16_write_dsp(DSP_CMD_SPEAKER_OFF);
}

/**
 * Program the DMA controller for 8-bit transfer
 */
void sb16_program_dma_8bit(void* buffer, uint16_t length, boolean auto_init) {
    uint32_t phys_addr = virtual_to_physical(buffer);
    uint8_t mode = auto_init ? DMA_MODE_AUTO_8BIT : DMA_MODE_SINGLE_8BIT;
    
    // Disable DMA channel
    port_byte_out(DMA_MASK_REG_8BIT, 0x04 | sb16.dma_8bit);
    
    // Clear flip-flop
    port_byte_out(DMA_FLIP_FLOP_8BIT, 1);
    
    // Set mode (single or auto)
    port_byte_out(DMA_MODE_REG_8BIT, mode | sb16.dma_8bit);
    
    // Set page, address, and count
    port_byte_out(DMA_PAGE_CHAN1, (phys_addr >> 16) & 0xFF);
    port_byte_out(DMA_ADDR_CHAN1, phys_addr & 0xFF);
    port_byte_out(DMA_ADDR_CHAN1, (phys_addr >> 8) & 0xFF);
    port_byte_out(DMA_COUNT_CHAN1, (length - 1) & 0xFF);
    port_byte_out(DMA_COUNT_CHAN1, ((length - 1) >> 8) & 0xFF);
    
    // Enable DMA channel
    port_byte_out(DMA_MASK_REG_8BIT, sb16.dma_8bit);
}

/**
 * Program the DMA controller for 16-bit transfer
 */
void sb16_program_dma_16bit(void* buffer, uint16_t length, boolean auto_init) {
    uint32_t phys_addr = virtual_to_physical(buffer);
    uint8_t mode = auto_init ? DMA_MODE_AUTO_16BIT : DMA_MODE_SINGLE_16BIT;
    
    // 16-bit DMA uses word counts, so adjust address and length
    phys_addr >>= 1;
    length >>= 1;
    
    // Disable DMA channel
    port_byte_out(DMA_MASK_REG_16BIT, 0x04 | (sb16.dma_16bit & 0x03));
    
    // Clear flip-flop
    port_byte_out(DMA_FLIP_FLOP_16BIT, 1);
    
    // Set mode (single or auto)
    port_byte_out(DMA_MODE_REG_16BIT, mode | (sb16.dma_16bit & 0x03));
    
    // Set page, address, and count
    port_byte_out(DMA_PAGE_CHAN5, (phys_addr >> 16) & 0xFF);
    port_byte_out(DMA_ADDR_CHAN5, phys_addr & 0xFF);
    port_byte_out(DMA_ADDR_CHAN5, (phys_addr >> 8) & 0xFF);
    port_byte_out(DMA_COUNT_CHAN5, (length - 1) & 0xFF);
    port_byte_out(DMA_COUNT_CHAN5, ((length - 1) >> 8) & 0xFF);
    
    // Enable DMA channel
    port_byte_out(DMA_MASK_REG_16BIT, sb16.dma_16bit & 0x03);
}

/**
 * Calculate time constant from sample rate
 */
uint8_t sb16_calculate_time_constant(uint16_t sample_rate, boolean stereo) {
    uint32_t channels = stereo ? 2 : 1;
    uint32_t time_constant = 256 - (1000000 / (sample_rate * channels));
    return (uint8_t)(time_constant & 0xFF);
}

/**
 * Play 8-bit sound
 */
void sb16_play_sound_8bit(uint16_t sample_rate, void* buffer, uint16_t length, boolean stereo, boolean signed_data, boolean auto_init) {
    uint8_t format = 0;
    
    // Prepare format byte
    if (stereo) format |= 0x20;
    if (signed_data) format |= 0x10;
    
    // Reset DSP first
    sb16_reset();
    
    // Turn speaker on
    sb16_speaker_on();
    
    // Program DMA
    sb16_program_dma_8bit(buffer, length, auto_init);
    
    // For SB16, we can use direct sample rate instead of time constant
    sb16_write_dsp(DSP_CMD_SET_SAMPLE_RATE);
    sb16_write_dsp((sample_rate >> 8) & 0xFF);  // High byte
    sb16_write_dsp(sample_rate & 0xFF);         // Low byte
    
    // Set transfer mode
    sb16_write_dsp(DSP_CMD_PLAY_8BIT | (auto_init ? 0x04 : 0x00));
    
    // Set sound format
    sb16_write_dsp(format);
    
    // Set length (one less than actual length)
    sb16_write_dsp((length - 1) & 0xFF);        // Low byte
    sb16_write_dsp(((length - 1) >> 8) & 0xFF); // High byte
}

/**
 * Play 16-bit sound
 */
void sb16_play_sound_16bit(uint16_t sample_rate, void* buffer, uint16_t length, boolean stereo, boolean signed_data, boolean auto_init) {
    uint8_t format = 0;
    
    // Prepare format byte
    if (stereo) format |= 0x20;
    if (signed_data) format |= 0x10;
    
    // 16-bit samples are typically signed
    if (!signed_data) {
        // Handle this case if needed
    }
    
    // Reset DSP first
    sb16_reset();
    
    // Turn speaker on
    sb16_speaker_on();
    
    // Program DMA
    sb16_program_dma_16bit(buffer, length, auto_init);
    
    // For SB16, we can use direct sample rate
    sb16_write_dsp(DSP_CMD_SET_SAMPLE_RATE);
    sb16_write_dsp((sample_rate >> 8) & 0xFF);  // High byte
    sb16_write_dsp(sample_rate & 0xFF);         // Low byte
    
    // Set transfer mode
    sb16_write_dsp(DSP_CMD_PLAY_16BIT | (auto_init ? 0x04 : 0x00));
    
    // Set sound format
    sb16_write_dsp(format);
    
    // Set length (one less than actual length, and in words for 16-bit)
    length >>= 1;  // Convert to words
    sb16_write_dsp((length - 1) & 0xFF);        // Low byte
    sb16_write_dsp(((length - 1) >> 8) & 0xFF); // High byte
}

/**
 * Stop playing 8-bit sound
 */
void sb16_stop_sound_8bit() {
    sb16_write_dsp(DSP_CMD_STOP_8BIT);
}

/**
 * Stop playing 16-bit sound
 */
void sb16_stop_sound_16bit() {
    sb16_write_dsp(DSP_CMD_STOP_16BIT);
}

/**
 * Acknowledge 8-bit interrupt
 */
void sb16_acknowledge_interrupt_8bit() {
    port_byte_in(SB16_DSP_READ_STATUS);
}

/**
 * Acknowledge 16-bit interrupt
 */
void sb16_acknowledge_interrupt_16bit() {
    port_byte_in(SB16_DSP_INT_ACK);
}

/**
 * Initialize the Sound Blaster 16
 */
boolean sb16_init() {
    // Try to detect the Sound Blaster 16
    if (!sb16_detect()) {
        return false;
    }
    
    // Set IRQ to default (IRQ 5)
    sb16_set_irq(sb16.irq);
    
    // Set master volume to default (75%)
    sb16_set_master_volume(12, 12);
    
    // Turn speaker on
    sb16_speaker_on();
    
    return true;
}

/**
 * Cleanup the Sound Blaster 16
 */
void sb16_cleanup() {
    // Turn speaker off
    sb16_speaker_off();
    
    // Stop any active playback
    sb16_stop_sound_8bit();
    sb16_stop_sound_16bit();
}