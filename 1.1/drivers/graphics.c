#include "graphics.h"

// VGA registers
#define VGA_AC_INDEX            0x3C0
#define VGA_AC_WRITE            0x3C0
#define VGA_AC_READ             0x3C1
#define VGA_MISC_WRITE          0x3C2
#define VGA_SEQ_INDEX           0x3C4
#define VGA_SEQ_DATA            0x3C5
#define VGA_DAC_INDEX_READ      0x3C7
#define VGA_DAC_INDEX_WRITE     0x3C8
#define VGA_DAC_DATA            0x3C9
#define VGA_MISC_READ           0x3CC
#define VGA_GC_INDEX            0x3CE
#define VGA_GC_DATA             0x3CF
#define VGA_CRTC_INDEX          0x3D4
#define VGA_CRTC_DATA           0x3D5
#define VGA_INSTAT_READ         0x3DA


#define VIDEO_ADDRESS 0xC00A0000 
// #define VIDEO_ADDRESS 0xA0000  

#define MAX_PALETTE_COLORS 256

// Track which colors are already in the palette
typedef struct {
    unsigned char r;  // 0-63
    unsigned char g;  // 0-63
    unsigned char b;  // 0-63
    int used;         // Usage count for LRU replacement
} PaletteEntry;

PaletteEntry palette[MAX_PALETTE_COLORS];


void init_palette() {
    // Initialize the first 16 colors as standard VGA colors
    uint8_t standard_colors[16][3] = {
        {0, 0, 0}, {0, 0, 42}, {0, 42, 0}, {0, 42, 42},
        {42, 0, 0}, {42, 0, 42}, {42, 21, 0}, {42, 42, 42},
        {21, 21, 21}, {21, 21, 63}, {21, 63, 21}, {21, 63, 63},
        {63, 21, 21}, {63, 21, 63}, {63, 63, 21}, {63, 63, 63}
    };

    for (int i = 0; i < 16; i++) {
        palette[i].r = standard_colors[i][0];
        palette[i].g = standard_colors[i][1];
        palette[i].b = standard_colors[i][2];
        palette[i].used = 1;

        // Set these in the VGA hardware
        port_byte_out(VGA_DAC_INDEX_WRITE, i);
        port_byte_out(VGA_DAC_DATA, palette[i].r);
        port_byte_out(VGA_DAC_DATA, palette[i].g);
        port_byte_out(VGA_DAC_DATA, palette[i].b);
    }

    // Mark remaining entries as unused
    for (int i = 16; i < MAX_PALETTE_COLORS; i++) {
        palette[i].used = 0;
    }
}


uint8_t get_color_index(uint8_t r, uint8_t g, uint8_t b) {
    r >>= 2;  // Convert 8-bit to 6-bit
    g >>= 2;
    b >>= 2;

    // Try to find an exact match
    for (int i = 0; i < MAX_PALETTE_COLORS; i++) {
        if (palette[i].used > 0 &&
            palette[i].r == r &&
            palette[i].g == g &&
            palette[i].b == b) {
            palette[i].used++;
            return i;
        }
    }

    // No exact match, find an unused entry
    for (int i = 16; i < MAX_PALETTE_COLORS; i++) {
        if (palette[i].used == 0) {
            palette[i].r = r;
            palette[i].g = g;
            palette[i].b = b;
            palette[i].used = 1;

            // Update the VGA palette
            wait_for_vsync();  // Avoid screen tearing
            port_byte_out(VGA_DAC_INDEX_WRITE, i);
            port_byte_out(VGA_DAC_DATA, r);
            port_byte_out(VGA_DAC_DATA, g);
            port_byte_out(VGA_DAC_DATA, b);

            return i;
        }
    }

    // Palette is full, replace the least-used color
    uint16_t min_used = palette[16].used;
    uint8_t min_index = 16;

    for (int i = 16; i < MAX_PALETTE_COLORS; i++) {
        if (palette[i].used < min_used) {
            min_used = palette[i].used;
            min_index = i;
        }
    }

    // Replace this entry
    palette[min_index].r = r;
    palette[min_index].g = g;
    palette[min_index].b = b;
    palette[min_index].used = 1;

    // Update the VGA palette
    wait_for_vsync();  // Avoid screen tearing
    port_byte_out(VGA_DAC_INDEX_WRITE, min_index);
    port_byte_out(VGA_DAC_DATA, r);
    port_byte_out(VGA_DAC_DATA, g);
    port_byte_out(VGA_DAC_DATA, b);

    return min_index;
}



// Wait for vertical retrace to avoid screen tearing
void wait_for_vsync() {
    // Wait until bit 3 of the Input Status Register 1 is set
    while ((port_byte_in(VGA_INSTAT_READ) & 0x08) == 0);
}

// Write to VGA registers
void write_registers(unsigned char *registers) {
    // MISC
    port_byte_out(VGA_MISC_WRITE, *registers++);
    
    // SEQUENCER
    for (unsigned char i = 0; i < 5; i++) {
        port_byte_out(VGA_SEQ_INDEX, i);
        port_byte_out(VGA_SEQ_DATA, *registers++);
    }
    
    // Unlock CRTC registers
    port_byte_out(VGA_CRTC_INDEX, 0x03);
    port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) | 0x80);
    port_byte_out(VGA_CRTC_INDEX, 0x11);
    port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) & ~0x80);
    
    // Make sure they remain unlocked
    registers[0x03] |= 0x80;
    registers[0x11] &= ~0x80;
    
    // CRTC
    for (unsigned char i = 0; i < 25; i++) {
        port_byte_out(VGA_CRTC_INDEX, i);
        port_byte_out(VGA_CRTC_DATA, *registers++);
    }
    
    // GRAPHICS
    for (unsigned char i = 0; i < 9; i++) {
        port_byte_out(VGA_GC_INDEX, i);
        port_byte_out(VGA_GC_DATA, *registers++);
    }
    
    // ATTRIBUTE
    for (unsigned char i = 0; i < 21; i++) {
        port_byte_in(VGA_INSTAT_READ);
        port_byte_out(VGA_AC_INDEX, i);
        port_byte_out(VGA_AC_WRITE, *registers++);
    }
    
    // Enable video output
    port_byte_in(VGA_INSTAT_READ);
    port_byte_out(VGA_AC_INDEX, 0x20);
}

// Mode 13h register values (320x200, 256 colors)
unsigned char mode13h_registers[] = {
    // MISC
    0x63,
    // SEQ
    0x03, 0x01, 0x0F, 0x00, 0x0E,
    // CRTC
    0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
    0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
    0xFF,
    // GRAPHICS
    0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
    0xFF,
    // ATTRIBUTE
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
    0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x41, 0x00, 0x0F, 0x00, 0x00
};

// Set up VGA Mode 13h
void set_video_mode() {
    write_registers(mode13h_registers);
    
    // Set up a basic VGA palette with some colors
    // set_palette();
}

// Set up a simple VGA palette (first 16 colors)
void set_palette() {
    port_byte_out(VGA_DAC_INDEX_WRITE, 0);  // Start with color 0
    
    // Basic 16 color palette (R, G, B values from 0-63)
    unsigned char palette[] = {
        0, 0, 0,       // Black (0)
        0, 0, 42,      // Blue (1)
        0, 42, 0,      // Green (2)
        0, 42, 42,     // Cyan (3)
        42, 0, 0,      // Red (4)
        42, 0, 42,     // Magenta (5)
        42, 21, 0,     // Brown (6)
        42, 42, 42,    // Light Gray (7)
        21, 21, 21,    // Dark Gray (8)
        21, 21, 63,    // Light Blue (9)
        21, 63, 21,    // Light Green (10)
        21, 63, 63,    // Light Cyan (11)
        63, 21, 21,    // Light Red (12)
        63, 21, 63,    // Light Magenta (13)
        63, 63, 21,    // Yellow (14)
        255, 255, 255     // White (15)
    };
    
    // Write the palette data
    for (int i = 0; i < 16 * 3; i++) {
        port_byte_out(VGA_DAC_DATA, palette[i]);
    }
}

// void draw_pixel(int x, int y, unsigned char color) {
//     if (x >= 0 && x < 320 && y >= 0 && y < 200) {
//         unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
//         vidmem[y * 320 + x] = color;
//     }
// }

void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x >= 0 && x < 320 && y >= 0 && y < 200) {
        unsigned char color_index = get_color_index(r, g, b);
        unsigned char *vidmm = (unsigned char *) VIDEO_ADDRESS;
        vidmm[y * 320 + x] = color_index;
    }
}


void draw_line(int x1, int y1, int x2, int y2, unsigned char color) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int e2;
    
    while (1) {
        // draw_pixel(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void draw_diagonal_line() {
    for (int i = 0; i < 100; i++) {
        draw_pixel(i, i, 0, 0, 100);  // White diagonal line
    }
}

void clear_screen_grpahics(char color) {
    unsigned char *vidmem = (unsigned char *) VIDEO_ADDRESS;
    for (int i = 0; i < 320 * 200; i++) {
        vidmem[i] = color; 
    }

    // memset((unsigned char *) VIDEO_ADDRESS, (unsigned char)0, 320*200);
    
    // for(int i = 0; i < 319; i++){
    //     for(int j = 0; j < 199; j++){
    //         draw_pixel(i, j, 255, 255, 255);
    //     }
    // }

    // int clear[3] = {0,0,0};

    for(int i = 0; i< 320; i++){
        for(int j = 0; j< 200; j++){
            int x = i;
            int y = j;
            int r = 0;
            int g = 0;
            int b = 0;
            draw_pixel(x, y, r, g, b);
        }
    }
}




#define REG_SCREEN_CTRL 0x3D4
#define REG_SCREEN_DATA 0x3D5

// Reset to text mode with proper paging support
void reset_video_mode() {
    // Text mode (mode 3) register values (80x25 text)
    unsigned char mode3_registers[] = {
        // MISC
        0x67,
        // SEQ
        0x03, 0x00, 0x03, 0x00, 0x02,
        // CRTC
        0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
        0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
        0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
        0xFF,
        // GRAPHICS
        0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
        0xFF,
        // ATTRIBUTE
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x0C, 0x00, 0x0F, 0x08, 0x00
    };



    // // Reset the VGA sequencer - first reset it
    port_byte_out(VGA_SEQ_INDEX, 0);
    port_byte_out(VGA_SEQ_DATA, 0x03);  // Reset sequencer
    
    // // Now use the full register set
    write_registers(mode3_registers);
    
    // // Wait for VGA to stabilize
    wait_for_vsync();
    
    // Reset memory mode for text mode
    port_byte_out(VGA_GC_INDEX, 6);
    port_byte_out(VGA_GC_DATA, 0x0E);  // Memory map for text mode
    
    // Don't clear video memory here - let your screen.c driver handle it with its functions
    // that are aware of your paged address (0xc00b8000 instead of 0xb8000)
    
    // Reset cursor position via VGA registers (this is independent of your memory paging)
    port_byte_out(REG_SCREEN_CTRL, 0x0E);  // Cursor location high byte
    port_byte_out(REG_SCREEN_DATA, 0x00);
    port_byte_out(REG_SCREEN_CTRL, 0x0F);  // Cursor location low byte
    port_byte_out(REG_SCREEN_DATA, 0x00);
    
    int offset =  0; // Convert from character cell offset to VGA cell offset
    port_byte_out(REG_SCREEN_CTRL, 14);           // High byte
    port_byte_out(REG_SCREEN_DATA, (offset >> 8) & 0x00FF);
    port_byte_out(REG_SCREEN_CTRL, 15);           // Low byte
    port_byte_out(REG_SCREEN_DATA, offset & 0x00FF);
    
    // Now use your screen.c functions which already use the correct paged address (0xc00b8000)
    // to initialize the text mode environment
    
    // clear_screen();  // This will use your paged VIDEO_ADDRESS
    
    // Set cursor to top-left using your functions (which are paging-aware)
    set_cursor(get_screen_offset(50, 50));
    set_cursor ( get_screen_offset (0 , 0));
    // printf("yoyoyoyoyoy\n\n");


    
    // // Manually reset video memory with explicit attribute bytes
    // char* vidmem = (char*)0xc00b8000;  // Use your paged address
    // for (int i = 0; i < 80 * 25 * 2; i += 2) {
    //     vidmem[i] = ' ';                // Space character
    //     vidmem[i + 1] = 0x0f; // Your attribute byte (typically 0x07)
    // }
    
    // // Set cursor position to top-left
    // port_byte_out(REG_SCREEN_CTRL, 0x0E);
    // port_byte_out(REG_SCREEN_DATA, 0x00);
    // port_byte_out(REG_SCREEN_CTRL, 0x0F);
    // port_byte_out(REG_SCREEN_DATA, 0x00);
    
}





void write_registers_text_mode(unsigned char* registers) {
    // MISC
    port_byte_out(VGA_MISC_WRITE, *registers);
    registers++;
    
    // SEQ
    for (unsigned char i = 0; i < 5; i++) {
        port_byte_out(VGA_SEQ_INDEX, i);
        port_byte_out(VGA_SEQ_DATA, *registers);
        registers++;
    }
    
    // Unlock CRTC registers
    port_byte_out(VGA_CRTC_INDEX, 0x03);
    port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) | 0x80);
    port_byte_out(VGA_CRTC_INDEX, 0x11);
    port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) & ~0x80);
    
    // Make sure they remain unlocked
    registers[0x03] |= 0x80;
    registers[0x11] &= ~0x80;
    
    // CRTC
    for (unsigned char i = 0; i < 25; i++) {
        port_byte_out(VGA_CRTC_INDEX, i);
        port_byte_out(VGA_CRTC_DATA, *registers);
        registers++;
    }
    
    // GRAPHICS
    for (unsigned char i = 0; i < 9; i++) {
        port_byte_out(VGA_GC_INDEX, i);
        port_byte_out(VGA_GC_DATA, *registers);
        registers++;
    }
    
    // ATTRIBUTE
    for (unsigned char i = 0; i < 21; i++) {
        port_byte_in(VGA_INSTAT_READ); // Reset attribute controller flip-flop
        port_byte_out(VGA_AC_INDEX, i);
        port_byte_out(VGA_AC_WRITE, *registers);
        registers++;
    }
    
    // Lock 16-color palette and enable display
    port_byte_in(VGA_INSTAT_READ);
    port_byte_out(VGA_AC_INDEX, 0x20);
}


#define KBRD_INTRFC 0x64

/* keyboard interface bits */
#define KBRD_BIT_KDATA 0 /* keyboard data is in buffer (output buffer is empty) (bit 0) */
#define KBRD_BIT_UDATA 1 /* user data is in buffer (command buffer is empty) (bit 1) */

#define KBRD_IO 0x60 /* keyboard IO port */
#define KBRD_RESET 0xFE /* reset CPU command */

#define bit(n) (1<<(n)) /* Set bit n to 1 */

/* Check if bit n in flags is set */
#define check_flag(flags, n) ((flags) & bit(n))

void reboot()
{
    uint8_t temp;

    asm volatile ("cli"); 
    do
    {
        temp = port_byte_in(KBRD_INTRFC); 
        if (check_flag(temp, KBRD_BIT_KDATA) != 0)
            port_byte_in(KBRD_IO); 
    } while (check_flag(temp, KBRD_BIT_UDATA) != 0);

    port_byte_out(KBRD_INTRFC, KBRD_RESET); /* pulse CPU reset line */
loop:
    asm volatile ("hlt"); /* if that didn't work, halt the CPU */
    goto loop; /* if a NMI is received, halt again */
}

// void reset_video_mode() {

//     set_video_mode();
//     // Text mode (mode 3) register values (80x25 text)
//     unsigned char mode3_registers[] = {
//         // MISC
//         0x67,
//         // SEQ
//         0x03, 0x00, 0x03, 0x00, 0x02,
//         // CRTC
//         0x5F, 0x4F, 0x50, 0x82, 0x55, 0x81, 0xBF, 0x1F,
//         0x00, 0x4F, 0x0D, 0x0E, 0x00, 0x00, 0x00, 0x00,
//         0x9C, 0x8E, 0x8F, 0x28, 0x1F, 0x96, 0xB9, 0xA3,
//         0xFF,
//         // GRAPHICS
//         0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0E, 0x00,
//         0xFF,
//         // ATTRIBUTE
//         0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//         0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
//         0x0C, 0x00, 0x0F, 0x08, 0x00
//     };

//     // // Reset the VGA sequencer
//     // port_byte_out(VGA_SEQ_INDEX, 0);
//     // port_byte_out(VGA_SEQ_DATA, 0x01);  // Synchronous reset
    
//     // Use our specialized text mode register writing function
//     write_registers_text_mode(mode3_registers);
    
//     // End synchronous reset (restart sequencer)
//     port_byte_out(VGA_SEQ_INDEX, 0);
//     port_byte_out(VGA_SEQ_DATA, 0x03);
    
//     // Wait for VGA to stabilize
//     wait_for_vsync();
    
//     // Critical: Make sure we're in text mode addressing
//     // Mode Control Register: Ensure bits 0-1 = 0 (Alpha mode)
//     port_byte_out(VGA_GC_INDEX, 5);
//     port_byte_out(VGA_GC_DATA, port_byte_in(VGA_GC_DATA) & ~0x03);
    
//     // Ensure we're using the correct memory map (bit 3=1, bit 2=0 for text)
//     // port_byte_out(VGA_GC_INDEX, 6);
//     // unsigned char mem_mode = port_byte_in(VGA_GC_DATA);
//     // port_byte_out(VGA_GC_DATA, (mem_mode & ~0x0C) | 0x04);  
    
//     // Make sure character map select is correct
//     port_byte_out(VGA_SEQ_INDEX, 3);
//     port_byte_out(VGA_SEQ_DATA, 0x00);  // Reset character generator
    
//     // Reset cursor position
//     set_cursor(get_screen_offset(0, 0));
    
//     // Clear the screen using your existing function
//     clear_screen();
    
//     // Test character output at specific positions
//     print_char('X', 0, 0, 0x0F);  // White on black at top-left
//     print_char('Y', 1, 0, 0x1F);  // White on blue at next position
    
//     // Print your test message
//     printf("Test text mode output\n");

//     // reboot();
// }