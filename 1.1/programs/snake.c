#include "snake.h"
// #include "../drivers/screen.h"
// #include "../drivers/keyboard.h"
// #include "../timer.h"

typedef struct {
    int x;
    int y;
} Point;

Point snake[SNAKE_MAX_LENGTH];
int snake_length = 3;
Point apple;
Direction current_direction = right;
int game_over = 0;
uint32_t lastKey;

// randomness functions
#define LCG_A 1103515245
#define LCG_C 12345
#define LCG_M (1 << 31) // 2^31
uint32_t seed = 2;

void srand_ticks() {
    // seed = getTicks(); // Use the current tick value as the seed
    // printf("seed: %d\n", seed);
}

uint32_t rand() {
    seed = (LCG_A * seed + LCG_C) % LCG_M;
    return seed;
}

uint32_t rand_range(uint32_t min, uint32_t max) {
    return min + (rand() % (max - min + 1));
}

void init_snake_game() {
    clear_screen();

    int start_x = GAME_WIDTH / 2;
    int start_y = GAME_HEIGHT / 2;

    for (int i = 0; i < snake_length; i++) {
        snake[i].x = start_x - i;
        snake[i].y = start_y;
    }
    
    // Place the first apple
   
    // apple.x = rand_range(0, MAX_COLS - 1);
    // apple.y = rand_range(0, MAX_ROWS - 1);
    apple.x = rand_range(0, 80 - 1);
    apple.y = rand_range(0, 25 - 1);
    

    game_over = 0;  
}

// Draw the snake and the apple
void draw_game() {
    clear_screen();

    // Draw the snake
    for (int i = 0; i < snake_length; i++) {
        print_char('O', snake[i].x, snake[i].y, GREEN_ON_GREEN);
    }

    // Draw the apple
    print_char('X', apple.x, apple.y, RED_ON_RED);
    printf("%d", snake[0].x);

    // Set the cursor to the bottom-right corner to avoid overwriting the game
    set_cursor(get_screen_offset(GAME_WIDTH - 1, GAME_HEIGHT - 1));

}

// Move the snake
void move_snake() {
    if (game_over) {
        print_at("Game Over! Press 'q' to quit.", 0, GAME_HEIGHT - 1);
        return;
    }

    // Update the body from tail to head
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    // Update the head based on the current direction
    switch (current_direction) {
        case up:
            snake[0].y--;
            break;
        case down:
            snake[0].y++;
            break;
        case left:
            snake[0].x--;
            break;
        case right:
            snake[0].x++;
            break;
        default:
            break;
    }

    // Check for collisions with the walls
    if (snake[0].x < 0 || snake[0].x >= GAME_WIDTH || snake[0].y < 0 || snake[0].y >= GAME_HEIGHT) {
        game_over = 1;
        set_cursor(get_screen_offset(3, 10));
        printf("Game Over");
    }

    // Check for collisions with itself
    for (int i = 1; i < snake_length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
            set_cursor(get_screen_offset(20, 20));
            printf("Game Over");
            break;
        }
    }

    // Check if the snake eats the apple
    if (snake[0].x == apple.x && snake[0].y == apple.y) {
        snake_length++;
        
        // apple.x = rand_range(0, MAX_COLS - 1);
        // apple.y = rand_range(0, MAX_ROWS - 1);

        apple.x = rand_range(0, 80 - 1);
        apple.y = rand_range(0, 25 - 1);
        
    }
}

void end_snake_game(){
    printf(" Game Over ... \n");
    printf("Final Score: %d\n", snake_length - 3);

    for(volatile int i = 0; i<1000000000; i++);
    set_colour(0x0,0x0);
    clear_screen();
    // setCurrentMode(MODE_CLI);
    setCurrentMode(4);
    set_colour(0x0, 0xf);
}

// Update the direction based on keyboard input
void update_direction(uint32_t key) {
    // printf("AYOOOO");
    lastKey = key;
    switch (key) {
        case 'w':
            if (current_direction != down) current_direction = up;
            break;
        case 's':
            if (current_direction != up) current_direction = down;
            break;
        case 'a':
            if (current_direction != right) current_direction = left;
            break;
        case 'd':
            if (current_direction != left) current_direction = right;
            break;
        case 'q':
            end_snake_game();
            // setCurrentMode(MODE_DEFUALT);
            // print(" Quitting Snake Game...\n");
            // for(volatile int i = 0; i<10000000; i++)
            // game_over = 1;
            break;
        default:
            break;
    }
}



// Start the snake game
void start_snake_game() {
    // init_keyboard();
    for (volatile int i = 0; i < 500000000; i++);
    // setCurrentMode(MODE_SNAKE);
    setCurrentMode(2);
    // port_byte_out(0x21, 0xff);
    // init_timer();
    init_snake_game();

    

    while (!game_over) {
        move_snake();
        draw_game();
        // printf("%d", getCurrentMode());
        // printf("%d", print_irq_routine(1));
        // printf("last char: %c", lastKey);
        
        
        // Simple delay for movement speed
        for (volatile int i = 0; i < 50000000; i++);
    }
    
    end_snake_game();
    
}
