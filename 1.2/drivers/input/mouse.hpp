#ifndef MOUSE_HPP
#define MOUSE_HPP

// library includes
#include "input_deivce.hpp"             // input device interface
#include "../../lib/stdint.hpp"         // mystd library
#include "../../lib/printf.hpp"         // printf library
#include "../../descriptors/idt.hpp"    // interrupt handler
#include "../../drivers/display/display.hpp"
#include "../../drivers/display/graphics.hpp"
#include "../../managers/process_manager.hpp"


// Mouse state structure
typedef struct {
    int x, y;        // Mouse cursor position
    int8_t dx, dy;   // Mouse movement deltas
    boolean left;       // Left button pressed
    boolean right;      // Right button pressed
    boolean middle;     // Middle button pressed
} MouseState;


#define MOUSE_IRQ 12
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define VIDMEM  ((uint16_t*)0xC00B8000)  // Updated video memory address for paging
#define VIDEO_MEM ((volatile uint16_t*)0xC00B8000)




class Mouse : public InputDevice{
public:
    Mouse();
    void init() override;
    void handleInterrupt(struct interrupt_register* regs) override;

    static void interrupt_wrapper(struct interrupt_register* regs);

    static void set_paint_colour(uint16_t pcol);
    static void update_mouse_cursor();

private:
    static void enable_mouse();
    static void disable_mouse();
};




#endif