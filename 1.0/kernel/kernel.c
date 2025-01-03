#include "../drivers/screen.h"



void main_(){
        
    // char * video_memory = ( char *) 0xb8000 ;
    // * video_memory = 'X';
    
    
    
    clear_screen();
    char * mess = "hey";
    print(mess);

    // char * video_memory = ( char *) 0xb8000 ;
    // * video_memory = 'X';


}
