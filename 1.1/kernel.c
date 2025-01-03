#include "screen.h"
#include "gdt.h"

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
}