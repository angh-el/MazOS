#ifndef PAINT_HPP
#define PAINT_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/input/keyboard.hpp"
#include "../drivers/input/mouse.hpp"
#include "../drivers/display/display.hpp"

class Paint{
public:
    static void paint();
    static void exit_paint();
    static void change_colour(char c);

};

#endif