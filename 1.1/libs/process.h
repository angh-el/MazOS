// #ifndef PROCESS_H
// #define PROCESS_H

// #include "stdint.h"

// #define MAX_PROCESSES 256

// // Process states
// typedef enum {
//     PROCESS_NEW,
//     PROCESS_READY,
//     PROCESS_RUNNING,
//     PROCESS_BLOCKED,
//     PROCESS_TERMINATED
// } process_state_t;

// // CPU context structure (for context switching)
// typedef struct {
//     uint32_t eax, ebx, ecx, edx;
//     uint32_t esi, edi, ebp;
//     uint32_t esp, eip;
//     uint32_t eflags;
//     uint32_t cr3; // Page directory base register (for memory mapping)
// } cpu_context_t;

// // Process Control Block (PCB)
// typedef struct process {
//     int pid;
//     process_state_t state;
//     cpu_context_t context;
//     uint32_t *stack;  // Pointer to process stack
//     struct process *next; // For scheduling (linked list or queue)
// } process_t;

// // Process Table
// extern process_t process_table[MAX_PROCESSES];
// extern process_t *current_process;

// // Function prototypes
// void init_processes();
// process_t *create_process(void (*entry_point)());
// void destroy_process(int pid);
// void schedule();
// void context_switch(process_t *next);
// void yield(); // Explicitly yield CPU to another process

// #endif // PROCESS_H



#ifndef PROCESS_H
#define PROCESS_H

#include "stdint.h"



// Define the task control block structure
typedef struct task_control_block {
    int pid;
    int state; // 0 = ready, 1 = running, 2 = finished
    unsigned int* kernel_stack_top;    // Pointer to the task's kernel stack
    unsigned int* virtual_address_space; // Pointer to the task's virtual address space (CR3)
    struct task_control_block* next_task;  // Pointer to the next task in the list
    unsigned int cpu_time;             // Total CPU time used by the task (for later)
    unsigned int priority;             // Task priority (optional, can add later)
    const char* task_name;             // Name of the task (optional)
    int time_remaining;
    int arrival_time;
    int tickets; // for lottery scehduling
    int completion_time;
    int start_time;
} task_control_block_t;

// extern task_control_block_t* current_task;


#endif // PROCESS_H



