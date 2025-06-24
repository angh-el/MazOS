#ifndef CLI_HPP
#define CLI_HPP

#include "snake.cpp"
#include "calculator.cpp"
#include "paint.cpp"

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"

#include "../drivers/display/display.hpp"
#include "../drivers/display/graphics.hpp"
#include "../drivers/input/keyboard.hpp"
#include "../drivers/input/mouse.hpp"
#include "../drivers/storage/fat32.hpp"

#include "../managers/process_manager.hpp"


#define COMMAND_BUFFER_SIZE 256  
#define HISTORY_SIZE 10  // Store up to 10 previous commands
#define UP_ARROW 0x48    // Scan code for up arrow
#define DOWN_ARROW 0x50  // Scan code for down arrow

// const uint32_t UNKNOWN = 0xFFFFFFFF;
const uint32_t uppp = 0xFFFFFFFF - 20;
const uint32_t downnn = 0xFFFFFFFF - 23;
char command[COMMAND_BUFFER_SIZE];  
int command_index = 0;  
char command_history[HISTORY_SIZE][COMMAND_BUFFER_SIZE];
int history_count = 0;  // Number of commands in history
int history_index = -1; // Current position in history when navigating


void init_cli(Fat32 f32);
void add_to_history(const char* cmd);
void help_function();
int string_compare(const char *str1, const char *str2);
void parse_command();
void print_command();
void handle_arrow_key(uint32_t scan_code);
void append_to_command(char c);

#endif