void kmain(void);

void kmain(void){
    char * video_memory = ( char *) 0xb8000 ;
    * video_memory = 'M';
    * (video_memory  + 2)= 'a';
    * (video_memory  + 4)= 'z';
    * (video_memory  + 6)= 'O';
    * (video_memory  + 8)= 'S';

}