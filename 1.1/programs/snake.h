#pragma once

#ifndef SNAKE_H
#define SNAKE_H

#define SNAKE_MAX_LENGTH 100
#define GAME_WIDTH 80
#define GAME_HEIGHT 25

#define GREEN_ON_GREEN 0x22
#define RED_ON_RED 0x44

typedef enum {
    up,
    down,
    left,
    right,
    none
} Direction;

void start_snake_game();
void update_direction(char key);

#endif // SNAKE_H
