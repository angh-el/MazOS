#ifndef SNAKE_HPP
#define SNAKE_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/input/keyboard.hpp"
#include "../drivers/display/display.hpp"
#include "../drivers/timer.hpp"

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


typedef struct {
    int x;
    int y;
} Point;

Point snake[SNAKE_MAX_LENGTH];
int snake_length = 24;
Point apple;
Direction current_direction = right;
int game_over = 0;
uint32_t lastKey;

// randomness functions
#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M (1 << 31) // 2^31
uint32_t seed = 2;


void start_snake_game();
void update_direction(uint32_t key);
void end_snake_game();
void move_snake();
void draw_game();
void init_snake_game();
uint32_t rand_range(uint32_t min, uint32_t max);
uint32_t rand();
void srand_ticks();

#endif