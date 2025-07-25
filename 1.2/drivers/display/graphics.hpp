#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

// library includes
#include "../../lib/stdint.hpp"
#include "../../lib/util.hpp"
#include "display.hpp"



// graphics classs definition
class Graphics{
public:
    //variables and constants
    static constexpr int VGA_AC_INDEX               = 0x3C0;
    static constexpr int VGA_AC_WRITE               = 0x3C0;
    static constexpr int VGA_AC_READ                = 0x3C1;
    static constexpr int VGA_MISC_WRITE             = 0x3C2;
    static constexpr int VGA_SEQ_INDEX              = 0x3C4;
    static constexpr int VGA_SEQ_DATA               = 0x3C5;
    static constexpr int VGA_DAC_INDEX_READ         = 0x3C7;
    static constexpr int VGA_DAC_INDEX_WRITE        = 0x3C8;
    static constexpr int VGA_DAC_DATA               = 0x3C9;
    static constexpr int VGA_MISC_READ              = 0x3CC;
    static constexpr int VGA_GC_INDEX               = 0x3CE;
    static constexpr int VGA_GC_DATA                = 0x3CF;
    static constexpr int VGA_CRTC_INDEX             = 0x3D4;
    static constexpr int VGA_CRTC_DATA              = 0x3D5;
    static constexpr int VGA_INSTAT_READ            = 0x3DA;      

    static constexpr unsigned int VIDEO_ADDRESS     = 0xC00A0000;

    // static constexpr int MAX_PALETTE_COLORS         = 256;
    // // Track which colors are already in the palette
    // typedef struct {
    //     unsigned char r;  // 0-63
    //     unsigned char g;  // 0-63
    //     unsigned char b;  // 0-63
    //     int used;         // Usage count for LRU replacement
    // } PaletteEntry;
    // static PaletteEntry palette[MAX_PALETTE_COLORS];



    // methods
    static void init();
    static void draw_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);
    static void draw_line(int x1, int y1, int x2, int y2, unsigned char color);
    static void draw_diagonal_line();
    static void clear_screen(char color);
    static void quit();
    static void reboot();

private:
    static void init_palette();
    static uint8_t get_color_index(uint8_t r, uint8_t g, uint8_t b);
    static void wait_for_vsync();
    static void write_registers(unsigned char *registers);
    static void set_palette();


};












#endif