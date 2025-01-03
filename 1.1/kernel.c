#include "screen.h"

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
}


void kmain(void){
    clear_screen();
    print_logo(); 

}