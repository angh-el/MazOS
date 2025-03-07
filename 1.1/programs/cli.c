#include "snake.h"
// #include "../drivers/keyboard.h"
#include "../libs/printf.h"
#include "../libs/stdint.h"

#include "../drivers/fat32.h"




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

    if(string_compare(funct, "clear")) {
        clear_screen();  
        return;
    } 
    if(string_compare(funct, "paint")) {
        paint();  
    } 
    if(string_compare(funct, "calculator")) {
        calculator();
    }
    if(string_compare(funct, "snake")) {
        start_snake_game();
    } 

    // file operations
    if(string_compare(funct, "cd")) {
        // printf("%s ", arg1);
        if(string_compare(arg1, "..")){
            cd_up();
            return;
        }
        else{
            char dirname[11] = {' '};
            for(int i =0; i<11; i++){
                if(arg1[i]!= 0){
                    dirname[i] = arg1[i];
                }
                else{
                    dirname[i] = ' ';
                }
            }
            cd(dirname);
        }
        return;
    }
    if(string_compare(funct, "ls")) {
        ls();
        return;
    }
    if(string_compare(funct, "cat")) {
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        read_file(filename);
        return;
    }
    if(string_compare(funct, "touch")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        create_entry(filename, 0x20);
        return;
        // create_entry(const char* name, uint8_t attributes)
    }
    if(string_compare(funct, "mkdir")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        create_entry(filename, 0x10);
        return;
    }
    if(string_compare(funct, "rmdir")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        delete_entry(filename);
        return;
    }
    if(string_compare(funct, "echo")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        overwrite_file(arg2, sizeof(arg2), filename);
        
        return;
    }
    if(string_compare(funct, "rm")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        delete_entry(filename);
        return;
    }
    if(string_compare(funct, "display")){
        setCurrentMode(5); // 5 for Graphics
        set_video_mode();
        clear_screen_grpahics(0);
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        draw_png_from_txt(filename);
        return;
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

