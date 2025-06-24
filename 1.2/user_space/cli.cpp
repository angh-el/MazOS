#include "cli.hpp"

Fat32 fat32;

void init_cli(Fat32 f32){
    // setCurrentMode(MODE_CLI);
    fat32 = f32;
    Keyboard::setCurrentMode(Mode::MODE_CLI);
    fat32.get_current_directory();
}


void add_to_history(const char* cmd) {
    // Only add non-empty commands
    if (cmd[0] == '\0') {
        return;
    }
    
    // Move all commands one position down
    for (int i = HISTORY_SIZE - 1; i > 0; i--) {
        for (int j = 0; j < COMMAND_BUFFER_SIZE; j++) {
            command_history[i][j] = command_history[i-1][j];
        }
    }
    
    // Add the new command at position 0
    for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
        command_history[0][i] = cmd[i];
    }
    
    // Update history count
    if (history_count < HISTORY_SIZE) {
        history_count++;
    }
    
    // Reset history index
    history_index = -1;
}




void help_function(){
    printf("\n");
    printf("================================HELP MANUAL====================================\n");
    printf("   Command     |    Description     |      Argument/s       |       Notes      \n");
    printf(" --------------+--------------------+-----------------------+------------------\n");
    printf("   clear       | Clears the screen  |         None          |                  \n");
    printf("   paint       |Opens the paint app |         None          | Press 'q' to quit\n");
    printf("   calc        |Opens the calculator|         None          | Press 'q' to quit\n");
    printf("   snake       |Opens the snake game|         None          | WASD for movement\n");
    printf("   cd arg1     | Change Directory   |arg1: name of directory|                  \n");
    printf("   ls          |List Directory entry|         None          |                  \n");
    printf("   cat arg1    |Output file contents|  arg1: name of file   | Without suffix   \n");
    printf("   touch arg1  | Create new file    |  arg1: name of file   | Without suffix   \n");
    printf("   mkdir arg1  |Create new directory|arg1: name of directory|                  \n");
    printf("   rmdir arg1  |Deletes a directory |arg1: name of directory|                  \n");
    printf(" echo arg1 arg2|  Write to file     |  arg1: name of file   | Without suffix   \n");
    printf("               |                    |  arg2: file contents  |                  \n");    
    printf("   rm arg1     | Delets a file      |  arg1: name of file   | Without suffix   \n");
    printf(" display arg1  |Open a .PNG file    |arg1: name of .png file| Without suffix   \n");
    printf("    reboot     | Reboot the OS      |         None          |                  \n");

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
    
    add_to_history(command);
    
    char funct[12] = {0};  // Stores the function name
    char arg1[20] = {0};   // Stores the first argument
    char arg2[226] = {0};  // Stores the second argument

    int i = 0, j = 0;
    int stage = 0;  // 0 = funct, 1 = arg1, 2 = arg2


    while (command[i] != '\0') {
        // printf("%c", command[i]);
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

    if(string_compare(funct, "help")){
        help_function();
        return;
    }
    if(string_compare(funct, "pwd")){
        char* current_dir;
        // memory_map_page((uint32_t)current_dir, (uint32_t)current_dir, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
        fat32.get_current_directory();
        // printf("Current directory: %s\n", current_dir);
        return;
    }
    if(string_compare(funct, "clear")) {
        Display::clear_screen();  
        return;
    } 
    if(string_compare(funct, "paint")) {
        paint();  
        return;
    } 
    if(string_compare(funct, "calc")) {
        calculator();
        return;
    }
    if(string_compare(funct, "snake")) {
        start_snake_game();
        return;
    } 

    // file operations
    if(string_compare(funct, "cd")) {
        // printf("%s ", arg1);
        if(string_compare(arg1, "..")){
            fat32.cd_up();
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
            fat32.cd(dirname);
        }
        return;
    }
    if(string_compare(funct, "ls")) {
        fat32.ls();
        return;
    }
    if(string_compare(funct, "cat")) {
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        fat32.read_file(filename);
        return;
    }
    if(string_compare(funct, "touch")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        fat32.create_entry(filename, 0x20);
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
        fat32.create_entry(filename, 0x10);
        return;
    }
    if(string_compare(funct, "rmdir")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        fat32.delete_entry(filename);
        return;
    }
    if(string_compare(funct, "echo")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        //////////////////////////////////
        fat32.overwrite_file((const uint8_t *)arg2, sizeof(arg2), filename);
        //////////////////////////////////

        return;
    }
    if(string_compare(funct, "rm")){
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        fat32.delete_entry(filename);
        return;
    }
    if(string_compare(funct, "display")){
        Keyboard::setCurrentMode(Mode::MODE_GRAPHICS); // 5 for Graphics
        // set_video_mode();
        Graphics::init();
        Graphics::clear_screen(0);
        char filename[11] = {' ',' ',' ',' ',' ',' ',' ',' ','T','X','T'};
        for(int i =0; i<11; i++){
            if(arg1[i]!= 0){
                filename[i] = arg1[i];
            }
        }
        fat32.draw_png_from_txt(filename);
        return;
    }
    if(string_compare(funct, "reboot")){
        Graphics::reboot();
        return;
    }
    if(string_compare(funct, "processes")){
        // printf("processes\n");
        // setCurrentMode(6);
        Display::clear_screen();
        // ProcessManager::draw_quarters();
        for(int i = 0; i < 50; i++){
            // process_1();
        }
        Display::set_cursor(Display::get_screen_offset(0,0));
        Display::clear_screen();
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
    // for(int i = 0; i < COMMAND_BUFFER_SIZE; i++){
    //     command[i] = '\0';
    // }
    // command_index=0;
}


void handle_arrow_key(uint32_t scan_code) {
    if (scan_code == UP_ARROW || scan_code == uppp) {
        // printf("yoyoyyoyoyiyiyi ");
        // Navigate to previous command
        if (history_index < history_count - 1) {
            history_index++;
            
            // Clear current command
            for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
                command[i] = '\0';
            }
            
            // Copy command from history
            for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
                command[i] = command_history[history_index][i];
                // if (command[i] == '\0') {
                //     break;
                // }
            }
            
            // Update command index
            command_index = 0;
            while (command[command_index] != '\0' && command_index < COMMAND_BUFFER_SIZE) {
                command_index++;
            }
            
            // Clear screen line and print updated command
            // (You may need to adjust this based on your terminal handling)
            // printf("\r                                                                               \r");
            printf("\n");
            for (int i = 0; i < 80; i++){
                printf("\b");
            }
            fat32.get_current_directory();
            print_command();
        }
    } else if (scan_code == DOWN_ARROW || scan_code == downnn) {
        // Navigate to next command
        if (history_index > 0) {
            history_index--;
            
            // Clear current command
            for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
                command[i] = '\0';
            }
            
            // Copy command from history
            for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
                command[i] = command_history[history_index][i];
                if (command[i] == '\0') {
                    break;
                }
            }
            
            // Update command index
            command_index = 0;
            while (command[command_index] != '\0' && command_index < COMMAND_BUFFER_SIZE) {
                command_index++;
            }
            
            // Clear screen line and print updated command
            // printf("\r                                                                               \r");
            printf("\n");
            for (int i = 0; i < 80; i++){
                printf("\b");
            }
            fat32.get_current_directory();
            print_command();
        } else if (history_index == 0) {
            // If we're at the most recent command, clear the input
            history_index = -1;
            for (int i = 0; i < COMMAND_BUFFER_SIZE; i++) {
                command[i] = '\0';
            }
            command_index = 0;
            
            // Clear screen line
            // printf("\r                                                                               \r");
            printf("\n");
            for (int i = 0; i < 80; i++){
                printf("\b");
            }
            fat32.get_current_directory();
        }
    }
}


void append_to_command(char c) {
    // printf("%c",c);
    // printf("index: %d\n", command_index);
    
    // if (c == UP_ARROW || c == DOWN_ARROW || c == 96 || c == uppp || c == downnn) {
    if (c == uppp || c == downnn) {
        handle_arrow_key(c);
        // printf("yyiyiy");
        return;
    }
    
    if(c == '\n'){
        // print_command();
        parse_command();
        // printf("done\n");
        fat32.get_current_directory();
        return;
    }
    if(c == '\b'){
        command_index--;
        return;
    }
    if (command_index < COMMAND_BUFFER_SIZE - 1) {  
        command[command_index] = c;
        command_index++;
        command[command_index] = '\0';  

    }
}