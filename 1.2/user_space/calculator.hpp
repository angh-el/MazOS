#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/input/keyboard.hpp"



class Calculator{
public:

    static int calculator();
    static void apppend_to_buffer(char c);
    static int evaluate_expression(const char *expr);

};


#endif