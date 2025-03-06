#include "snake.h"
// #include "../drivers/keyboard.h"
#include "../libs/printf.h"
#include "../libs/stdint.h"




#define COMMAND_BUFFER_SIZE 256  

char command[COMMAND_BUFFER_SIZE];  
int command_index = 0;  

void init_cli(){
    // setCurrentMode(MODE_CLI);
    
    setCurrentMode(4);
}



int string_compare(const char *str1, const char *str2) {
    
    int i = 0;
    
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return 0;  // Strings do not match
        }
        i++;
    }
    
    // Ensure both strings end at the same place
    return (str1[i] == '\0' && str2[i] == '\0');
}


void parse_command() {
    char funct[12] = {0};  // Stores the function name
    char arg1[20] = {0};   // Stores the first argument
    char arg2[226] = {0};  // Stores the second argument

    int i = 0, j = 0;
    int stage = 0;  // 0 = funct, 1 = arg1, 2 = arg2

    while (command[i] != '\0') {
        if (command[i] == ' ' && stage < 2) {  
            stage++;
            j = 0;  
        } else {
            if (stage == 0 && j < sizeof(funct) - 1) {
                funct[j++] = command[i];
                funct[j] = '\0';  
            } else if (stage == 1 && j < sizeof(arg1) - 1) {
                arg1[j++] = command[i];
                arg1[j] = '\0';
            } else if (stage == 2 && j < sizeof(arg2) - 1) {
                arg2[j++] = command[i];  
                arg2[j] = '\0';  
            }
        }
        i++;
    }

    // Print results
    // printf("Function: %s\n", funct);
    // printf("Arg1: %s\n", arg1);
    // printf("Arg2: %s\n", arg2);

    // Clear the buffer
    for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
        command[i] = '\0';
    }
    command_index = 0;

    
    if(string_compare(funct, "paint")) {
        paint();  
    } 
    if(string_compare(funct, "calculator")) {
        calculator();
    }
    if(string_compare(funct, "snake")) {
        

        start_snake_game();
        // return;
        // test();
        
    
    
    } 
    else {
        printf("Unknown command: %s\n", funct);
    }

}



void print_command() {
    // printf("BIGUPPP");
    for (int i = 0; i < command_index; i++) {
        printf("%c", (command[i]));  
    }
    for(int i = 0; i < COMMAND_BUFFER_SIZE; i++){
        command[i] = '\0';
    }
    command_index=0;
}

void append_to_command(char c) {
    // printf("%c",c);
    // printf("index: %d\n", command_index);
    if(c == '\n'){
        // print_command();
        parse_command();
        // printf("done\n");
        return;
    }
    if (command_index < COMMAND_BUFFER_SIZE - 1) {  
        command[command_index] = c;
        command_index++;
        command[command_index] = '\0';  
    }
}

