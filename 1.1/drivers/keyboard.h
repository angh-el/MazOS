#pragma once

#include "../libs/stdint.h"
#include "../libs/printf.h"
#include "../libs/util.h"



void init_keyboard();
void handle_keyboard(struct interrupt_register* regs);



