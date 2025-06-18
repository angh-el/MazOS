#include "process_manager.hpp"


#define KERNEL_STACK_SIZE 4096      // 4KB stack per process
#define TIME_QUANTUM 10
#define NUM_TASKS 4


extern "C" void switch_to_task(task_control_block_t* next_task);

void ProcessManager::init(){
    current_task = (task_control_block_t*)kmalloc(sizeof(task_control_block_t));
    if (current_task == NULL) {
        return;
    }

    // Set up the TCB fields for the first task (running since boot)
    current_task->kernel_stack_top = nullptr;  // Kernel stack (to be set later)
    current_task->virtual_address_space = nullptr; // Set to the current address space (CR3)
    current_task->next_task = nullptr;  // No next task yet
    current_task->state = 1;  // Assume the task is "running" (1 for running)
    current_task->cpu_time = 0;  // No CPU time used yet
    current_task->priority = 0;  // Default priority (can be changed later)
    current_task->task_name = "Initial Task";  // Name of the task

}

void ProcessManager::context_switch(){
    // Call the switch_to_task function to switch from current_task to next_task
    switch_to_task(next_task);
}

void ProcessManager::task_startup(void (*task_function)()) {
    // Call the actual task function after initialisation
    task_function();
}


// Create a new kernel task
task_control_block_t* ProcessManager::create_task(void (*task_function)(), int pid, int exec_time, int tickets) {
    // Allocate memory for the task's TCB
    
    task_control_block_t* new_task = (task_control_block_t*)kmalloc(sizeof(task_control_block_t));
    if (new_task == NULL) {
        return nullptr;
    }

    // Allocate memory for the task's kernel stack
    uint32_t* new_stack = (uint32_t*)kmalloc(KERNEL_STACK_SIZE);
    

    // Manually map the allocated stack pages
    for (uint32_t i = 0; i < KERNEL_STACK_SIZE; i += 0x1000) {
        uint32_t phys_addr = physical_memory_alloc_page_frame();
        if (!phys_addr) {
            kfree(new_task);
            kfree(new_stack);
            return nullptr; // Handle memory allocation failure
        }
        memory_map_page((uint32_t)new_stack + i, phys_addr, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    }
    

    uint32_t *current_page_dir = memory_get_current_page_dir();
    uint32_t page_dir_idx = ((uint32_t)new_stack) >> 22;
    uint32_t page_table_idx = (((uint32_t)new_stack) >> 12) & 0x3FF;
    uint32_t *page_table = REC_PAGETABLE(page_dir_idx);
    
    if (!(page_table[page_table_idx] & PAGE_FLAG_PRESENT)) {
        // printf("Error: Stack memory is not mapped!\n");
    }



    // Initialize the new task's TCB
    new_task->kernel_stack_top = new_stack;
    new_task->virtual_address_space = current_task->virtual_address_space;  // Inherit the current address space
    new_task->state = 0;  // Initially set as "ready" 
    new_task->cpu_time = 0;
    new_task->priority = 0;
    new_task->task_name = "New Task";
    new_task->next_task = NULL;  // No next task yet

    new_task->pid = pid;
    new_task->time_remaining = exec_time;
    new_task->arrival_time = getTicks();
    new_task->tickets = tickets;
    

    // Set up the kernel stack for the new task    
    // uint32_t* stack_ptr = new_stack + KERNEL_STACK_SIZE - 1;
    uint32_t* stack_ptr = (uint32_t*)((uint8_t*)new_stack + KERNEL_STACK_SIZE - sizeof(uint32_t));

    uint32_t stack_page = (uint32_t)stack_ptr & ~0xFFF;  // Get the start of the page
    uint32_t* stack_page_table = REC_PAGETABLE(stack_page >> 22);
    uint32_t stack_page_idx = (stack_page >> 12) & 0x3FF;
   
    uint32_t task_function_addr = (uint32_t)task_function;

    memory_map_page((uint32_t)task_startup, (uint32_t)task_startup - KERNEL_START, PAGE_FLAG_PRESENT );
    memory_map_page((uint32_t)task_function, (uint32_t)task_function - KERNEL_START, PAGE_FLAG_PRESENT);

    *stack_ptr-- = (uint32_t)task_startup;  // Return to task_startup first
    *stack_ptr-- = (uint32_t)task_function;  // After startup, jump to the real task function

    // Save the stack pointer in the task's TCB
    new_task->kernel_stack_top = (void*)stack_ptr;

    // Set up the TCB and return it
    return new_task;
}