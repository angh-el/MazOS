#ifndef CLI_HPP
#define CLI_HPP

#include "snake.hpp"
#include "calculator.hpp"
#include "paint.hpp"

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


class CLI {
public:
    static void init(Fat32 f32);
    static void add_to_history(const char* cmd);
    static void help_function();
    static int string_compare(const char *str1, const char *str2);
    static void parse_command();
    static void print_command();
    static void handle_arrow_key(uint32_t scan_code);
    static void append_to_command(char c);

};

#endif