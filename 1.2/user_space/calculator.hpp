#ifndef CALCUlATOR_HPP
#define CALCUlATOR_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/input/keyboard.hpp"

char input[100];
int input_index = 0;

int calculator();
void apppend_to_buffer(char c);


#endif