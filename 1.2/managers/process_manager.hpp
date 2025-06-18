#ifndef PROCESS_MANAGER_HPP
#define PROCESS_MANAGER_HPP

// library includes and external functions needed
#include "../lib/stdint.hpp"
#include "memory_manager.hpp"
#include "heap_manager.hpp"


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


class ProcessManager {
public:
    // member variables
    task_control_block_t* current_task = nullptr;  // Global pointer to the current task
    task_control_block_t* next_task = nullptr;     // Pointer to the next task
    task_control_block_t* task_list[4];
    int current_task_index = 0;
    const char* selected_algorithm = "FCFS";

    // member functions
    static void init();
    static void context_switch();
    static void task_startup(void (*task_function)());
    static task_control_block_t* create_task(void (*task_function)(), int pid, int exec_time, int tickets);


};










#endif