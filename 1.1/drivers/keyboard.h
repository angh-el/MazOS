#pragma once

#include "../libs/stdint.h"
#include "../libs/printf.h"
#include "../libs/util.h"
#include "../programs/calculator.h"
#include "../programs/snake.h"
#include "../programs/cli.h"


typedef enum{
    MODE_DEFUALT,       // 0
    MODE_CALCULATOR,    // 1
    MODE_SNAKE,         // 2
    MODE_PAINT,
    MODE_CLI
}Mode;


void init_keyboard();
void handle_keyboard(struct interrupt_register* regs);


void setCurrentMode(Mode mode);

Mode getCurrentMode();
