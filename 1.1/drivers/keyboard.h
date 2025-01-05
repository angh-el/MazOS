#pragma once

#include "../libs/stdint.h"
#include "../libs/printf.h"
#include "../libs/util.h"

#include "../idt.h"

void init_keyboard();
void handle_keyboard(struct interrupt_register* registers);



