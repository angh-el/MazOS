#include "keyboard.hpp"
#include "../../lib/printf.hpp"

Keyboard * Keyboard::instance = nullptr;

static Mode currentMode;
static bool shift, caps;



Keyboard::Keyboard() {
    // empty constructor
};



void Keyboard::init(){
    instance = this;
    DescriptorTables::IDT::irq_install_handler(1, interrupt_wrapper);
}

void Keyboard::interrupt_wrapper(struct interrupt_register* regs){
    char keyCode = port_byte_in(0x60) & 0x7f;
    // if key is held or not
    char press = port_byte_in(0x60) & 0x80;

    handleKey(keyCode, press);
}

// void Keyboard::handleInterrupt(){
void Keyboard::handleInterrupt(struct interrupt_register* regs){
    // what key is being pressed
    char keyCode = port_byte_in(0x60) & 0x7f;

    // if key is held or not
    char press = port_byte_in(0x60) & 0x80;

    printf("%c", keyCode);
    handleKey(keyCode, press);
}

void Keyboard::handleKey(uint8_t keyCode, bool press){
    switch (keyCode){
    case 1:
    case 29:
    case 56:
    case 54:
    case 59:
    case 60:
    case 61:
    case 62:
    case 63:
    case 64:
    case 65:
    case 66:
    case 67:
    case 68:
    case 87:
    case 88:
    //case 96:
        break;
    
    case 72:
        if(press == 0)
        CLI::append_to_command(UP);
        break;
    case 80:
        if(press == 0)
        CLI::append_to_command(DOWN);
        break;
    
    case 42:
        if(press == 0)shift = true;
        else shift = false;
        break;
    case 58:
        if(!caps && press == 0) caps = true;
        else if(caps && press == 0) caps = false;
        break;

    default:
        if(press == 0){
            if(shift || caps ){
                if(currentMode == Mode::MODE_DEFUALT){
                    printf("%c",uppercase[keyCode]);
                }
                if(currentMode == Mode::MODE_CALCULATOR){
                    Calculator::apppend_to_buffer(uppercase[keyCode]);
                }
                if(currentMode == Mode::MODE_SNAKE){
                    Snake::update_direction((char)uppercase[keyCode]);
                }
                if(currentMode == Mode::MODE_PAINT){
                    Paint::change_colour(uppercase[keyCode]);
                }
                if(currentMode == Mode::MODE_CLI){
                    // if(keyCode != 72 && keyCode != 80){
                    // if(keyCode != 72 && keyCode != 80 && keyCode!= 96){
                        printf("%c",uppercase[keyCode]);
                        CLI::append_to_command(uppercase[keyCode]);
                    // }
                }
                if(currentMode == Mode::MODE_GRAPHICS){
                    if(uppercase[keyCode] == 'Q'){
                        Graphics::reboot();
                    }
                }

            }
            else{
                switch (currentMode) {
                    case Mode::MODE_DEFUALT:
                        printf("%c", lowercase[keyCode]);
                        break;

                    case Mode::MODE_CALCULATOR:
                        Calculator::apppend_to_buffer(lowercase[keyCode]);
                        break;

                    case Mode::MODE_SNAKE:
                        Snake::update_direction(lowercase[keyCode]);
                        // printf("SNAKE %c"s, lowercase[keyCode]);
                        break;

                    case Mode::MODE_PAINT:
                        Paint::change_colour(lowercase[keyCode]);
                        // printf("PAINT %c", lowercase[keyCode]);
                        break;

                    case Mode::MODE_CLI:
                    //    if(keyCode != 72 && keyCode != 80 && keyCode!= 96){
                            printf("%c", lowercase[keyCode]);
                            // printf("%d", keyCode);
                            CLI::append_to_command(lowercase[keyCode]);
                        // }
                        break;
                    case Mode::MODE_GRAPHICS:
                        if(lowercase[keyCode] == 'q'){
                            Graphics::reboot();
                        }
                        break;
                    case Mode::MODE_PROCESSES:
                        if(lowercase[keyCode] == 'q'){
                            Display::clear_screen();
                            // get_current_directory();
                            setCurrentMode(Mode::MODE_CLI);
                        }
                        break;

                    default:
                        // printf("OTHER %c", lowercase[keyCode]);
                        // Handle unexpected mode if necessary
                        break;
                }

            } 
        }
        break;
    }
}

void Keyboard::setCurrentMode(Mode mode){
    currentMode = mode;
}

Mode Keyboard::getCurrentMode() {
    return currentMode;
}