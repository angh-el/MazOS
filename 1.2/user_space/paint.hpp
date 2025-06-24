#ifndef PAINT_HPP
#define PAINT_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/input/keyboard.hpp"
#include "../drivers/input/mouse.hpp"
#include "../drivers/display/display.hpp"


void paint();
void exit_paint();
void change_colour(char c);

#endif