#pragma once
#include "../libs/stdint.h"

// #ifndef SNAKE_H
// #define SNAKE_H

#define SNAKE_MAX_LENGTH 100
#define GAME_WIDTH 80
#define GAME_HEIGHT 25

#define GREEN_ON_GREEN 0x22
#define RED_ON_RED 0x44


// int game_over = 0;

typedef enum {
    up,
    down,
    left,
    right,
    none
} Direction;

void start_snake_game();
void update_direction(uint32_t key);

// #endif // SNAKE_H
