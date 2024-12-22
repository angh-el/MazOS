void main (){
    // Create a pointer to a char , and point it to the first text cell of
    // video memory (i.e. the top - left of the screen )
    char * video_memory = ( char *) 0xb8000 ;
    // At the address pointed to by video_memory , store the character ’X’
    // (i.e. display ’X’ in the top - left of the screen ).
    * video_memory = 'X';
    

    // The text to display
    // const char *text = "Some Sample Text";

    // // Iterate through the text and write each character to video memory
    // for (int i = 0; text[i] != '\0'; i++) {
    //     video_memory[i * 2] = text[i];        // Write the character
    //     // video_memory[i * 2 + 1] = 0x07;      // Write the attribute (white on black)
    // }

}