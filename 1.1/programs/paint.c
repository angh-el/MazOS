#include "paint.h"


void paint(){
    // clear screen
    set_colour(0xf, 0xf);
    clear_screen();
    
    setCurrentMode(MODE_PAINT);

    init_mouse();

    set_cursor(get_screen_offset(0,0));
    set_colour(0xf, 0);
    printf(" 0-Black ");
    set_colour(0xf, 1);
    printf(" 1-Blue ");
    set_colour(0xf, 2);
    printf(" 2-Green ");
    set_colour(0xf, 3);
    printf(" 3-Cyan ");
    set_colour(0xf, 4);
    printf(" 4-Red ");
    set_colour(0xf, 5);
    printf(" 5-Magenta ");
    set_colour(0xf, 6);
    printf(" 6-Brown ");
    set_colour(0xf, 4);
    printf(" 7-LightGrey ");
    set_colour(0xf, 8);
    printf(" 8-DarkGrey ");
    set_colour(0xf, 9);
    printf(" 9-LightBlue ");
    set_colour(0xf, 0xa);
    printf(" A-LightGreen ");
    set_colour(0xf, 0xb);
    printf(" B-LightCyan ");
    set_colour(0xf, 0xc);
    printf(" C-LightRed ");
    set_colour(0xf, 0xd);
    printf(" D-LightMagenta ");
    set_colour(0xf, 0xe);
    printf(" E-Yellow ");
    set_colour(0xf, 0x1);
    printf(" F-White ");
    
}


void exit_paint(){
    set_colour(0x0, 0xf);
    clear_screen();
    setCurrentMode(MODE_CLI);
}

// void set_paint_colour(uint16_t pcol){
//     paint_colour = pcol;
// }

void change_colour(char c){
    switch(c){
        case 'q':
            exit_paint();
            break;
        case '0':
            set_paint_colour((uint16_t)0x00);
            break;
        case '1':
            set_paint_colour((uint16_t)0x10);
            break;
        case '2':
            set_paint_colour((uint16_t)0x20);
            break;
        case '3':
            set_paint_colour((uint16_t)0x30);
            break;
        case '4':
            set_paint_colour((uint16_t)0x40);
            break;
        case '5':
            set_paint_colour((uint16_t)0x50);
            break;
        case '6':
            set_paint_colour((uint16_t)0x60);
            break;
        case '7':
            set_paint_colour((uint16_t)0x70);
            break;
        case '8':
            set_paint_colour((uint16_t)0x80);
            break;
        case '9':
            set_paint_colour((uint16_t)0x90);
            break;
        case 'A':
        case 'a':
            set_paint_colour((uint16_t)0xa0);
            break;
        case 'B':
        case 'b':
            set_paint_colour((uint16_t)0xb0);
            break;
        case 'C':
        case 'c':
            set_paint_colour((uint16_t)0xc0);
            break;
        case 'D':
        case 'd':
            set_paint_colour((uint16_t)0xd0);
            break;
        case 'E':
        case 'e':
            set_paint_colour((uint16_t)0xe0);
            break;
        case 'F':
        case 'f':
            set_paint_colour((uint16_t)0xf0);
            break;
        default:
            break;
    }
    
   
    
}