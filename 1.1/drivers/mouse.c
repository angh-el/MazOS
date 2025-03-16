#include "mouse.h"
#include "../idt.h"
#include "screen.h"  


#define MOUSE_IRQ 12
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDMEM  ((uint16_t*)0xC00B8000)  // Updated video memory address for paging

static MouseState mouse_state = {40, 12, 0, 0, false, false, false};
static uint8_t mouse_cycle = 0;  
static int8_t mouse_bytes[3];    

static char last_char = ' ';  // Stores the last character under the cursor


#define VIDEO_MEM ((volatile uint16_t*)0xC00B8000)
// Stores the last drawn character & color at previous cursor position
uint16_t last_cell_content = 0x0720;  // Default: space with normal color
int last_x = 0, last_y = 0;           // Store last position

uint16_t paint_colour = 0xe0;

void set_paint_colour(uint16_t pcol){
    paint_colour = pcol;
}

void update_mouse_cursor() {
    // Restore the last cell's original content (unless it was clicked)
    if (!(mouse_state.left || mouse_state.right)) {
        int last_offset = (last_y * SCREEN_WIDTH) + last_x;
        VIDEO_MEM[last_offset] = last_cell_content;
    }

    // Reduce movement speed
    mouse_state.dx /= 10;
    mouse_state.dy /= 10;

    // Update cursor position
    mouse_state.x += mouse_state.dx;
    mouse_state.y -= mouse_state.dy;  // Y-axis is inverted

    // Ensure cursor stays in bounds
    if (mouse_state.x < 0) mouse_state.x = 0;
    if (mouse_state.x >= SCREEN_WIDTH) mouse_state.x = SCREEN_WIDTH - 1;
    if (mouse_state.y < 0) mouse_state.y = 0;
    if (mouse_state.y >= SCREEN_HEIGHT) mouse_state.y = SCREEN_HEIGHT - 1;

    // Compute new cursor position
    int new_offset = (mouse_state.y * SCREEN_WIDTH) + mouse_state.x;

    // **PERMANENTLY** change color if clicked
    if (mouse_state.left) {
        VIDEO_MEM[new_offset] = (paint_colour << 8) | (VIDEO_MEM[new_offset] & 0xFF);  // Green
        return;
    }
    if (mouse_state.right) {
        VIDEO_MEM[new_offset] = (0x40 << 8) | (VIDEO_MEM[new_offset] & 0xFF);  // Red
        return;
    }

    // Store the current cell's content before drawing cursor
    last_cell_content = VIDEO_MEM[new_offset];
    last_x = mouse_state.x;
    last_y = mouse_state.y;

    // Draw grey cursor
    VIDEO_MEM[new_offset] = (0x70 << 8) | ' ';
}


// Mouse IRQ handler
void handle_mouse_interrupt() {
    uint8_t mouse_data = port_byte_in(0x60);  // Read mouse byte
    // printf("MOUSE\n");
    switch (mouse_cycle) {
        case 0:
            if (!(mouse_data & 0x08)) return;  // Ignore if bit 3 is not set
            mouse_bytes[0] = mouse_data;
            mouse_cycle++;
            break;
        case 1:
            mouse_bytes[1] = mouse_data;
            mouse_cycle++;
            break;
        case 2:
            mouse_bytes[2] = mouse_data;
            mouse_cycle = 0;  // Reset for next packet

            // Process mouse packet
            // mouse_state.dx = (mouse_bytes[0] & 0x10) ? (mouse_bytes[1] | 0xFFFFFF00) : mouse_bytes[1];
            // mouse_state.dy = (mouse_bytes[0] & 0x20) ? (mouse_bytes[2] | 0xFFFFFF00) : mouse_bytes[2];
            
            
            // mouse_state.dx = (mouse_bytes[0] & 0x10) ? (int8_t)mouse_bytes[1] : mouse_bytes[1];
            // mouse_state.dy = (mouse_bytes[0] & 0x20) ? (int8_t)mouse_bytes[2] : mouse_bytes[2];

            // mouse_state.dx = (int8_t)mouse_bytes[1];  
            // mouse_state.dy = (int8_t)mouse_bytes[2];

            mouse_state.dx = (mouse_bytes[0] & 0x10) ? (mouse_bytes[1] - 256) : mouse_bytes[1];
            mouse_state.dy = (mouse_bytes[0] & 0x20) ? (mouse_bytes[2] - 256) : mouse_bytes[2];

            // mouse_state.dx = (int8_t)mouse_bytes[1];
            // mouse_state.dy = -(int8_t)mouse_bytes[2];  // Inverted Y-axis

            // printf("Raw: b0: %02X, b1: %02X, b2: %02X\n", mouse_bytes[0], mouse_bytes[1], mouse_bytes[2]);
            mouse_state.left = mouse_bytes[0] & 0x01;
            mouse_state.right = mouse_bytes[0] & 0x02;
            mouse_state.middle = mouse_bytes[0] & 0x04;

            // if (mouse_state.left) {
            //     printf("green\n");
            // } else if (mouse_state.right) {
            //     printf("red\n");
            // }
            
            // printf("dx: %d, dy:%d\n", mouse_state.dx, mouse_state.dy);

            update_mouse_cursor();
            break;
    }
}

// Enable mouse device
void enable_mouse() {

    port_byte_out(0x64, 0xA8);  // Enable auxiliary device (mouse)
    port_byte_out(0x64, 0x20);  // Get controller command byte
    uint8_t status = port_byte_in(0x60);
    status |= 0x02;  // Enable IRQ12
    port_byte_out(0x64, 0x60);
    port_byte_out(0x60, status);

    // // **Set sample rate**
    // port_byte_out(0x64, 0xD4);
    // port_byte_out(0x60, 0xF3);  // Set sample rate command
    // port_byte_in(0x60);         // Wait for ACK (0xFA)
    
    // port_byte_out(0x64, 0xD4);
    // port_byte_out(0x60, 20);  // Set sample rate (try 40 or 60)
    // port_byte_in(0x60);         // Wait for ACK (0xFA)


    // // **SET MOUSE RESOLUTION **
    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 0xE8);  // Set resolution command
    port_byte_in(0x60);         // Wait for ACK (0xFA)
    
    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 3);  // 0 = lowest, 3 = highest
    port_byte_in(0x60);         // Wait for ACK (0xFA)

    
    // Send initialization commands to the mouse
    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 0xF4);  // Enable mouse
    port_byte_in(0x60);  // Acknowledge response
}

void disable_mouse() {
    port_byte_out(0x64, 0xA7);  // Disable the auxiliary device (mouse)
}


// Initialize mouse
void init_mouse() {
    enable_mouse();
    irq_install_handler(MOUSE_IRQ, handle_mouse_interrupt);
}



