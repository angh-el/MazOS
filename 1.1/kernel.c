#include "drivers/screen.h"
#include "gdt.h"
#include "idt.h"
#include "libs/printf.h"
#include "drivers/keyboard.h"

#include "timer.h"

void kmain(void);

void print_logo(){
    print("\n\n\n\n\n\n\n\n");
    print("            ##     ##    ###    ########     #######   ######  \n");
    print("            ###   ###   ## ##        ##     ##     ## ##    ## \n");
    print("            #### ####  ##   ##      ##      ##     ## ##       \n");
    print("            ## ### ## ##     ##    ##       ##     ##  ######  \n");
    print("            ##     ## #########   ##        ##     ##       ##\n");
    print("            ##     ## ##     ##  ##         ##     ## ##    ## \n");
    print("            ##     ## ##     ## ########     #######   ######  \n");
    print("\n\n\n\n\n\n\n\n");
}


void kmain(void){
    clear_screen();
    print_logo(); 

    init_gdt();
    print("Successfullly enabled the gdt ting\n");
    print("\n");
    
    init_idt();
    print("Successfully enabled interrupts\n\n");
    // // print("\nDividing by 0 ...\n");
    // // print(2/0);

    // // printf("hello\n\t\tyoooooo does this work test tes %d", 69);
    // // printf("XXXX\n");

    init_keyboard();
    print("Successfully enabled keyboard driver\n");

    set_cursor(get_screen_offset(1, 1));
    // // printf("test test");

    // init_timer();

    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    // print_irq_routine(0);
    for(;;);
}