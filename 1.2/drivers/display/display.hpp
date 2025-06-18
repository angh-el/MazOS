#ifndef DISPLAY_HPP
#define DISPLAY_HPP

// include external libraries and what not...
#include "../../lib/stdint.hpp"     // for uint32_t types
#include "../../lib/util.hpp"       // for memcopy

// Display class definiton
class Display {
public:
    static constexpr unsigned int VIDEO_ADDRESS = 0xc00b8000;
    // static constexpr int VIDEO_ADDRESS = 0xcb8000;
    static constexpr int MAX_ROWS = 25;
    static constexpr int MAX_COLS = 80;
    static constexpr char WHITE_ON_BLACK = 0x0f;

    static constexpr int REG_SCREEN_CTRL = 0x3D4;
    static constexpr int REG_SCREEN_DATA = 0x3D5;

    // public static method
    static void clear_screen();
    static void print_char(char character, int col, int row, char attributeByte);
    static void print_at(const char* message, int col, int row);
    static void print(const char* message);

    static void set_cursor(int offset);
    static int get_cursor();
    static int get_screen_offset(int col, int row);

    static void set_colour(uint8_t bg, uint8_t fg);


private:
    static int handle_scrolling(int cursor_offset);
    static void memory_copy ( char * source , char * dest , int no_bytes );
};



#endif