#include "display.hpp"

uint16_t colour = (0x0 << 4) | 0xf;

void Display::set_colour(uint8_t bg, uint8_t fg){
    colour = (bg << 4) | (fg & 0x0F);
}

void Display::memory_copy ( char * source , char * dest , int no_bytes ) {
    for (int i = 0; i < no_bytes; ++i ){
        // *( dest + i) = *( source + i );
        dest[i] = source[i];
    }
}

int Display::get_screen_offset(int col, int row) {
    if (col < 0 || col >= MAX_COLS || row < 0 || row >= MAX_ROWS) {
        return -1; 
    }

    return (row * MAX_COLS + col) * 2; 
}

int Display::get_cursor () {
    port_byte_out ( REG_SCREEN_CTRL , 14);
    int offset = port_byte_in ( REG_SCREEN_DATA ) << 8;
    port_byte_out ( REG_SCREEN_CTRL , 15);
    offset += port_byte_in ( REG_SCREEN_DATA );
    return offset *2;
}

void Display::set_cursor(int offset) {
    offset /= 2; // Convert from character cell offset to VGA cell offset
    port_byte_out(REG_SCREEN_CTRL, 14);           // High byte
    port_byte_out(REG_SCREEN_DATA, (offset >> 8) & 0x00FF);
    port_byte_out(REG_SCREEN_CTRL, 15);           // Low byte
    port_byte_out(REG_SCREEN_DATA, offset & 0x00FF);
}

void Display::clear_screen () {
    int row = 0;
    int col = 0;
    for ( row = 0; row < MAX_ROWS; row ++) {
        for ( col = 0; col < MAX_COLS; col ++) {
            // print_char(' ', col, row, WHITE_ON_BLACK);
            print_char(' ', col, row, colour);
            
        }
    }
    // Move the cursor back to the top left .
    set_cursor ( get_screen_offset (0 , 0));
} 

/* Advance the text cursor , scrolling the video buffer if necessary . */
int Display::handle_scrolling ( int cursor_offset ) {
    // If the cursor is within the screen , return it unmodified .
    if ( cursor_offset < MAX_ROWS * MAX_COLS *2) {
        return cursor_offset ;
    }

    /* Shuffle the rows back one . */
    int i;
    for (i =1; i < MAX_ROWS ; i ++) {
        memory_copy (   (char*)(get_screen_offset (0 , i) + VIDEO_ADDRESS) ,
                        (char*)(get_screen_offset (0 ,i -1) + VIDEO_ADDRESS) ,
                        MAX_COLS *2
        );
    }

    /* Blank the last line by setting all bytes to 0 */
    char * last_line = (char*)(get_screen_offset (0 , MAX_ROWS -1) + VIDEO_ADDRESS) ;
    for (i =0; i < MAX_COLS *2; i ++) {
        last_line [ i] = 0;
    }

    // Move the offset back one row , such that it is now on the last
    // row , rather than off the edge of the screen .
    cursor_offset -= 2* MAX_COLS ;
    
    // Return the updated cursor position .
    return cursor_offset ;
}

/* Print a char on the screen at col , row , or at cursor position */
void Display::print_char(char character , int col , int row , char attribute_byte ) {
    char *vidmem = (char *) VIDEO_ADDRESS ;
    
    if (attribute_byte == 0) {
        attribute_byte = WHITE_ON_BLACK ;
    }
    
    /* Get the video memory offset for the screen location */
    // int offset ;
    int offset;

    /* If col and row are non - negative , use them for offset . */
    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);
        /* Otherwise , use the current cursor position . */
    } 
    else {
        offset = get_cursor();
    }
    
    if ( character == '\n') {
        int rows = offset / (2* MAX_COLS);
        offset = get_screen_offset(79 , rows);
        character = '\b';
        offset+=4;
    }
    if(character == '\b'){
        offset -= 2; // Move back by one character (2 bytes per character cell)
        vidmem[offset] = ' '; // Clear the character
        vidmem[offset + 1] = attribute_byte; // Retain the same attribute
    }
    else {
      
        // vidmem[offset] = (const char*) character ;
        vidmem[offset] =  character ;
        vidmem[offset+1] = attribute_byte ;
        // set_cursor(get_cursor());
        offset += 2;
    }
    
    // Update the offset to the next character cell , which is
    // two bytes ahead of the current cell .
    
    // Make scrolling adjustment , for when we reach the bottom
    // of the screen .
    offset = handle_scrolling(offset);

    // Update the cursor position on the screen device .
    set_cursor(offset);
}

void Display::print_at (const char* message , int col , int row ) {
    // Update the cursor if col and row not negative .
    if ( col >= 0 && row >= 0) {
        set_cursor ( get_screen_offset ( col , row ));
    
    }

    while(*message){
        print_char(*message, col, row, colour);
        message++;
    }    
}

void Display::print(const char* message){
    print_at(message , -1, -1);
}