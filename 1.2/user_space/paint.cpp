#include "paint.hpp"


void Paint::paint(){
    // clear screen
    Display::set_colour(0xf, 0xf);
    Display::clear_screen();
    
    Keyboard::setCurrentMode(Mode::MODE_PAINT);

    // Mouse::init();

    Display::set_cursor(Display::get_screen_offset(0,0));
    Display::set_colour(0xf, 0);
    printf(" 0-Black ");
    Display::set_colour(0xf, 1);
    printf(" 1-Blue ");
    Display::set_colour(0xf, 2);
    printf(" 2-Green ");
    Display::set_colour(0xf, 3);
    printf(" 3-Cyan ");
    Display::set_colour(0xf, 4);
    printf(" 4-Red ");
    Display::set_colour(0xf, 5);
    printf(" 5-Magenta ");
    Display::set_colour(0xf, 6);
    printf(" 6-Brown ");
    Display::set_colour(0xf, 4);
    printf(" 7-LightGrey ");
    Display::set_colour(0xf, 8);
    printf(" 8-DarkGrey ");
    Display::set_colour(0xf, 9);
    printf(" 9-LightBlue ");
    Display::set_colour(0xf, 0xa);
    printf(" A-LightGreen ");
    Display::set_colour(0xf, 0xb);
    printf(" B-LightCyan ");
    Display::set_colour(0xf, 0xc);
    printf(" C-LightRed ");
    Display::set_colour(0xf, 0xd);
    printf(" D-LightMagenta ");
    Display::set_colour(0xf, 0xe);
    printf(" E-Yellow ");
    Display::set_colour(0xf, 0x1);
    printf(" F-White ");
    
}


void Paint::exit_paint(){
    Display::set_colour(0x0, 0xf);
    Display::clear_screen();
    Keyboard::setCurrentMode(Mode::MODE_CLI);
}

// void set_paint_colour(uint16_t pcol){
//     paint_colour = pcol;
// }

void Paint::change_colour(char c){
    switch(c){
        case 'q':
            exit_paint();
            break;
        case '0':
            Mouse::set_paint_colour((uint16_t)0x00);
            break;
        case '1':
            Mouse::set_paint_colour((uint16_t)0x10);
            break;
        case '2':
            Mouse::set_paint_colour((uint16_t)0x20);
            break;
        case '3':
            Mouse::set_paint_colour((uint16_t)0x30);
            break;
        case '4':
            Mouse::set_paint_colour((uint16_t)0x40);
            break;
        case '5':
            Mouse::set_paint_colour((uint16_t)0x50);
            break;
        case '6':
            Mouse::set_paint_colour((uint16_t)0x60);
            break;
        case '7':
            Mouse::set_paint_colour((uint16_t)0x70);
            break;
        case '8':
            Mouse::set_paint_colour((uint16_t)0x80);
            break;
        case '9':
            Mouse::set_paint_colour((uint16_t)0x90);
            break;
        case 'A':
        case 'a':
            Mouse::set_paint_colour((uint16_t)0xa0);
            break;
        case 'B':
        case 'b':
            Mouse::set_paint_colour((uint16_t)0xb0);
            break;
        case 'C':
        case 'c':
            Mouse::set_paint_colour((uint16_t)0xc0);
            break;
        case 'D':
        case 'd':
            Mouse::set_paint_colour((uint16_t)0xd0);
            break;
        case 'E':
        case 'e':
            Mouse::set_paint_colour((uint16_t)0xe0);
            break;
        case 'F':
        case 'f':
            Mouse::set_paint_colour((uint16_t)0xf0);
            break;
        default:
            break;
    }
    
   
    
}