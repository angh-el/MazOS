#include "libs/printf.h"
#include "libs/util.h"
#include "idt.h"
#include "drivers/screen.h"
#include "timer.h"
#include "libs/process.h"

// #include "programs/snake.h"





uint64_t ticks;
uint32_t freq = 100;

task_control_block_t* curent_task;

void onIrq0(struct interrupt_register *regs){
    ticks += 1;
    // printf("%d ", ticks);

    // if(curent_task){
    //     curent_task->cpu_time++;
    // }

    // if((uint32_t)ticks % (uint32_t)10 == (uint32_t)0){
    //     // switch every 10 ticks
    //     schedule();
    // }

    // if (!game_over) {
    // if (!0) {
    //     move_snake();
    //     draw_game();
    // } else {
    //     end_snake_game();
    //     // stop_timer(snake_timer_callback);  // Stop the timer when game ends
    //     kill_timer();
    // }


}

void init_timer(){
    ticks = 0;
    irq_install_handler(0,&onIrq0);

    //119318.16666 Mhz
    uint32_t divisor = 1193180/freq;

    //0011 0110
    port_byte_out(0x43,0x36);
    port_byte_out(0x40,(uint8_t)(divisor & 0xFF));
    port_byte_out(0x40,(uint8_t)((divisor >> 8) & 0xFF));
}

void kill_timer(){
    clear_irq_handler(0);
}


int getTicks(){
    return ticks;
}

void setFreq(uint32_t frequency){
    freq = frequency;
}


unsigned long start_time = 0;

unsigned long get_current_time() {
    return ticks;  
}

void timer_start() {
    start_time = get_current_time(); 
}

void timer_end() {
    unsigned long end_time = get_current_time();  
    unsigned long elapsed_time = end_time - start_time;
    print_elapsed_time(elapsed_time);  
}

void timer_interrupt_handler() {
}



void print_elapsed_time(unsigned long elapsed_time) {
    printf("Elapsed time: %lu\n", elapsed_time);
}
