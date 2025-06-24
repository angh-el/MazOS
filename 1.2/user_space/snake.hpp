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

// randomness functions
#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M (1 << 31) // 2^31



class Snake{
public:
    static void start_snake_game();
    static void update_direction(uint32_t key);
    static void end_snake_game();
    static void move_snake();
    static void draw_game();
    static void init_snake_game();
    static uint32_t rand_range(uint32_t min, uint32_t max);
    static uint32_t rand();
    static void srand_ticks();

};

#endif