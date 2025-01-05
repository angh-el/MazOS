#pragma once

#include "stdint.h"

void putc (const char* c);
void puts (const char* str);
void printf(const char * fmt, ...);
int * printf_number(int*, int, boolean, int);