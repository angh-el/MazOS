#include "../drivers/screen.h"
#include "process.h"
#include "kernel_memory.h"  
#include "../drivers/fat32.h"   // for strncmp 

#define KERNEL_STACK_SIZE 4096  // 4KB stack per process
#define TIME_QUANTUM 10         // time quantum for round robin
#define NUM_TASKS 4

task_control_block_t* current_task = NULL;  // Global pointer to the current task
task_control_block_t* next_task = NULL;     // Pointer to the next task

task_control_block_t* task_list[4];
int current_task_index = 0;

const char* selected_algorithm = "FCFS";

extern void switch_to_task(task_control_block_t* next_task);  // Declare the external assembly function

void initialise_multitasking() {
    current_task = (task_control_block_t*)kmalloc(sizeof(task_control_block_t));
    if (current_task == NULL) {
        return;
    }

    // Set up the TCB fields for the first task (running since boot)
    current_task->kernel_stack_top = NULL;  // Kernel stack (to be set later)
    current_task->virtual_address_space = NULL; // Set to the current address space (CR3)
    current_task->next_task = NULL;  // No next task yet
    current_task->state = 1;  // Assume the task is "running" (1 for running)
    current_task->cpu_time = 0;  // No CPU time used yet
    current_task->priority = 0;  // Default priority (can be changed later)
    current_task->task_name = "Initial Task";  // Name of the task
}


// Function to trigger a context switch
void context_switch() {
    // Call the switch_to_task function to switch from current_task to next_task
    switch_to_task(next_task);
}


