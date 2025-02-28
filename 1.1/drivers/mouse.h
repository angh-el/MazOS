#ifndef MOUSE_H
#define MOUSE_H

#include "../libs/stdint.h"
#include "../libs/util.h"

// Mouse state structure
typedef struct {
    int x, y;        // Mouse cursor position
    int8_t dx, dy;   // Mouse movement deltas
    boolean left;       // Left button pressed
    boolean right;      // Right button pressed
    boolean middle;     // Middle button pressed
} MouseState;

void init_mouse();
void handle_mouse_interrupt();

#endif
