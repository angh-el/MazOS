#include "screen.h"

// define the functions:
void print_char( char character , int col , int row , char attribute_byte );
int handle_scrolling ( int cursor_offset );
void memory_copy ( char * source , char * dest , int no_bytes );
// void clear_screen ();
// void print(const char  *message);
void print_at ( char * message , int col , int row );
uint16_t get_cursor();
void set_cursor(uint16_t offset);
uint16_t get_screen_offset(int col, int row);



unsigned char port_byte_in ( unsigned short port ) {
    // A handy C wrapper function that reads a byte from the specified port
    // "=a" ( result ) means : put AL register in variable RESULT when finished
    // "d" ( port ) means : load EDX with port
    unsigned char result ;
    __asm__("in %%dx, %%al" : "=a" (result) : "d" (port));
    return result ;
}

void port_byte_out ( unsigned short port , unsigned char data ) {
    // "a" ( data ) means : load EAX with data
    // "d" ( port ) means : load EDX with port
    __asm__("out %%al, %%dx" : : "a" (data), "d" ( port));
}

unsigned short port_word_in ( unsigned short port ) {
    unsigned short result ;
    __asm__ ("in %%dx, %%ax" : "=a " ( result ) : "d" ( port ));
    return result ;
}

void port_word_out ( unsigned short port , unsigned short data ) {
    __asm__ ("out %%ax, %%dx" : :"a" ( data ), "d" ( port ));
}

/* Copy bytes from one place to another . */
void memory_copy ( char * source , char * dest , int no_bytes ) {
    for (int i = 0; i < no_bytes; i++ ){
        *( dest + i) = *( source + i );
    }
}



uint16_t get_screen_offset(int col, int row) {
    // Ensure col and row are within bounds
    if (col < 0 || col >= MAX_COLS || row < 0 || row >= MAX_ROWS) {
        return -1; // Return an invalid offset if inputs are out of bounds
    }

    // Calculate the offset in bytes for the given (col, row)
    return (row * MAX_COLS + col) * 1; // Multiply by 2 because each cell is 2 bytes
}


uint16_t get_cursor () {
    // The device uses its control register as an index
    // to select its internal registers , of which we are
    // interested in:
    // reg 14: which is the high byte of the cursor ’s offset
    // reg 15: which is the low byte of the cursor ’s offset
    // Once the internal register has been selected , we may read or
    // write a byte on the data register .
    port_byte_out ( REG_SCREEN_CTRL , 14);
    int offset = port_byte_in ( REG_SCREEN_DATA ) << 8;
    port_byte_out ( REG_SCREEN_CTRL , 15);
    offset += port_byte_in ( REG_SCREEN_DATA );
    // Since the cursor offset reported by the VGA hardware is the
    // number of characters , we multiply by two to convert it to
    // a character cell offset .
    return offset *1;
}

void set_cursor(uint16_t offset) {
    offset /= 1; // Convert from character cell offset to VGA cell offset
    port_byte_out(REG_SCREEN_CTRL, 14);           // High byte
    port_byte_out(REG_SCREEN_DATA, (offset >> 8) & 0x00FF);
    port_byte_out(REG_SCREEN_CTRL, 15);           // Low byte
    port_byte_out(REG_SCREEN_DATA, offset & 0x00FF);

}



// void clear_screen () {

//     int row = 0;
//     int col = 0;
//     /* Loop through video memory and write blank characters . */
//     for ( row = 0; row < MAX_ROWS ; row ++) {
//         for ( col = 0; col < MAX_COLS ; col ++) {
//             // print_char('S', col, row, WHITE_ON_BLACK);
//             print_at('S', col, row);
//         }
//     }
//     // Move the cursor back to the top left .
//     set_cursor ( get_screen_offset (0 , 0));
//     // set_cursor ( get_screen_offset (80-1 , 25-1));

//     // char *vidmem = (char *) VIDEO_ADDRESS ;

//     // for(int i = 0; i < MAX_COLS*MAX_ROWS; i+=2){
//     //     vidmem[i] = ' ';
//     //     vidmem[i+1] = WHITE_ON_BLACK;
//     // }
//     // set_cursor (0);
// } 


// /* Advance the text cursor , scrolling the video buffer if necessary . */
// int handle_scrolling ( int cursor_offset ) {
//     // If the cursor is within the screen , return it unmodified .
//     if ( cursor_offset < MAX_ROWS * MAX_COLS *2) {
//         return cursor_offset ;
//     }

//     /* Shuffle the rows back one . */
//     int i;
//     for (i =1; i < MAX_ROWS ; i ++) {
//         memory_copy (   get_screen_offset (0 , i) + VIDEO_ADDRESS ,
//                         get_screen_offset (0 ,i -1) + VIDEO_ADDRESS ,
//                         MAX_COLS *2
//         );
//     }

//     /* Blank the last line by setting all bytes to 0 */
//     char * last_line = get_screen_offset (0 , MAX_ROWS -1) + VIDEO_ADDRESS ;
//     for (i =0; i < MAX_COLS *2; i ++) {
//         last_line [ i] = 0;
//     }

//     // Move the offset back one row , such that it is now on the last
//     // row , rather than off the edge of the screen .
//     cursor_offset -= 2* MAX_COLS ;
    
//     // Return the updated cursor position .
//     return cursor_offset ;
// }

// /* Print a char on the screen at col , row , or at cursor position */
// void print_char(char character , int col , int row , char attribute_byte ) {
//     char * video_memory = ( char *) 0xb8000;
//     * video_memory = 'X';

//     /* Create a byte ( char ) pointer to the start of video memory */
//     // char *vidmem = (char *) VIDEO_ADDRESS ;
//     uint16_t *vidmem = (uint16_t *) VIDEO_ADDRESS ;
    
//     /* If attribute byte is zero , assume the default style . */
//     if (attribute_byte == 0) {
//         attribute_byte = WHITE_ON_BLACK ;
//     }
    
//     /* Get the video memory offset for the screen location */
//     // int offset ;
//     uint16_t offset;

//     /* If col and row are non - negative , use them for offset . */
//     if (col >= 0 && row >= 0) {
//         offset = get_screen_offset(col, row);
//         /* Otherwise , use the current cursor position . */
//     } 
//     else {
//         offset = get_cursor();
//     }
    
//     // If we see a newline character , set offset to the end of
//     // current row , so it will be advanced to the first col
//     // of the next row.
//     if ( character == '\n') {
//         int rows = offset / (2* MAX_COLS );
//         offset = get_screen_offset(79 , rows );
//     // Otherwise , write the character and its attribute byte to
//     // video memory at our calculated offset .
//     } 
//     else {
//         // vidmem[offset] = character ;
//         // vidmem[offset+1] = attribute_byte ;

//         vidmem[offset ] = (character << 8) | attribute_byte;
//     }
    
//     // Update the offset to the next character cell , which is
//     // two bytes ahead of the current cell .
//     offset += 5;
    
//     // Make scrolling adjustment , for when we reach the bottom
//     // of the screen .
//     // offset = handle_scrolling(offset);

//     // Update the cursor position on the screen device .
//     set_cursor(offset);
// }

// void print_at ( char *message , int col , int row ) {
//     // Update the cursor if col and row not negative .
//     if ( col >= 0 && row >= 0) {
//         set_cursor ( get_screen_offset ( col , row ));
    
//     }

//     // Loop through each char of the message and print it.
//     int i = 0;
//     while (message[i] != '\0') {
//         print_char( message[ i++] , col , row , WHITE_ON_BLACK );
//     }
    
// }


// void print(char  *message){
//     print_at(message , -1, -1);

//     // print_char('A', -1,-1, WHITE_ON_BLACK);
//     // print_char('B', -1,-1, WHITE_ON_BLACK);
//     // print_char('C', -1,-1, WHITE_ON_BLACK);


//     // char * video_memory = ( char *) 0xb8000 + 4;
//     // * video_memory = 'X';
// }




/////////////////////////////////////////////////////////////////////
/*ATTEMPT 2*/

// define the global variables
uint16_t col = 0;
uint16_t row = 0;
// char* const vidmem =  (char* const)0xb8000;
uint16_t* const vidmem =  (uint16_t* const) 0xB8000;
const uint16_t defaultColour = (COLOUR_BLACK << 8) | (COLOUR_WHITE << 12);
uint16_t colour = defaultColour;

// uint16_t colour = (uint16_t)WHITE_ON_BLACK;

void clear_screen(){
    row = 0;
    col = 0;

    for(uint16_t r = 0; r <= MAX_ROWS; r++){
        for(uint16_t c = 0; c <= MAX_COLS; c++){
            vidmem[(r * MAX_COLS + c)] = ('h') | colour;
           }
    }

    // char * video_memory = ( char *) 0xb8000 ;
    // * vidmem = 'X';
}


void scroll(){
    for(uint16_t r = 0; r < MAX_ROWS; r++){
        for(uint16_t c = 0; c < MAX_COLS; c++){
            vidmem[(r-1) * MAX_COLS + c] = vidmem[r * MAX_COLS + c];
        }
    }

    for(uint16_t c = 0; c < MAX_COLS; c++){
        vidmem[(MAX_ROWS-1) * MAX_COLS + c] = ' ' | colour;
    }
}

void new_line(){
    if(row < MAX_ROWS - 1){
        row++;
        col = 0;
    }
    else{
        scroll();
        col = 0;
    }
}

// line = row
// column = col
// height = MAX_ROWS
// width = MAX_COLS

void print(char* message){

      if ( *message == '\0') {
        * vidmem = 'X';
    }

    while(*message){
        
        switch(*message){
            case '\n':
                new_line();
                break;
            case '\r':
                col = 0;
                break;
            case '\t':
                if(col == MAX_COLS) new_line();

                uint16_t tab = 4 - (col % 4);
                while(tab != 0 ){
                    vidmem[row * MAX_COLS + (col++)] = ' ' | colour;
                    tab--;
                }
                break;
            default:
                if(col == MAX_COLS) new_line();

                vidmem[row * MAX_COLS + (col++)] = *message | colour;
                break;
        }

        message++;
    }
}