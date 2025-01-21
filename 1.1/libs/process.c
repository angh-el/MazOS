#include "process.h"

extern void save_process_state(PCB *process);
extern void load_process_state(PCB *process);

PCB *process_queue[MAX_PROCESSES];
static int queue_head = 0;
static int queue_tail = 0;
int current_process_idx = -1;


// Initialize the process system
void init_processes() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        process_queue[i] = NULL;
    }
}


// // Create a new process
// PCB* create_process(uint32_t entry_point) {
//     PCB *new_process = (PCB *) kmalloc(sizeof(PCB)); // Allocate memory for PCB
//     new_process->pid = queue_tail;                   // Assign a PID
//     new_process->entry_point = entry_point;           // Set the entry point
//     new_process->state = 0;                           // Set initial state (READY)
    
//     // Allocate stack and set page directory
//     new_process->stack = kmalloc(KERNEL_STACK_SIZE);
//     new_process->page_dir = memory_alloc_page_dir();  // Function for page directory allocation
    
//     // Add the new process to the queue
//     process_queue[queue_tail] = new_process;
//     queue_tail = (queue_tail + 1) % MAX_PROCESSES;
    
//     // Perform a context switch to start the new process
//     // schedule();

//     return new_process;
// }


// #define STACK_SIZE 4096  // Stack size for each process

// PCB* create_process(uint32_t entry_point) {
//     // Allocate memory for the PCB
//     PCB* pcb = (PCB*) kmalloc(sizeof(PCB));
//     if (pcb == NULL) {
//         printf("Out of memory for PCB");
//     }

//     // Allocate memory for the process stack
//     uint32_t* stack = (uint32_t*) kmalloc(STACK_SIZE);
//     if (stack == NULL) {
//         // panic("Out of memory for process stack");
//         printf("errror stack is null");
//     }
    

//     // Map PCB and stack pages
//     memory_map_page((uint32_t)pcb, (uint32_t)pcb, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
//     // memory_map_page((uint32_t)stack, (uint32_t)stack, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

//     // memory_map_page((uint32_t)(stack + STACK_SIZE - 1), (uint32_t)(stack + STACK_SIZE - 1), PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

//     printf("stack: %d\n", stack);

//     // Initialize the PCB fields
//     pcb->entry_point = entry_point;

//     // Set up the initial stack pointer (stack grows downward)
//     // pcb->stack = (uint32_t)(stack + STACK_SIZE / sizeof(uint32_t));
//     // pcb->stack = (uint32_t)(stack + STACK_SIZE) - sizeof(uint32_t);  // Set to the last valid address in the stack
//     // pcb->stack = (uint32_t)((uint8_t*)stack + STACK_SIZE);
//     pcb->stack = stack;

//     printf("stack: %d\n", pcb->stack);

//     // Initialize registers and other state (this is an example, adjust as necessary)
//     pcb->cpu_state.eax = 0;
//     pcb->cpu_state.ebx = 0;
//     pcb->cpu_state.ecx = 0;
//     pcb->cpu_state.edx = 0;
//     pcb->cpu_state.esi = 0;
//     pcb->cpu_state.edi = 0;
//     pcb->cpu_state.ebp = 0;

//     // Set up initial stack (could be different depending on how your context switching works)
//     pcb->cpu_state.esp = (uint32_t)(stack + STACK_SIZE / sizeof(uint32_t));
//     pcb->cpu_state.eip = entry_point;

//     // Return the new process's PCB
//     return pcb;
// }




// PCB* create_process(uint32_t entry_point) {
//     // Allocate memory for the PCB
//     PCB* pcb = (PCB*) kmalloc(sizeof(PCB));
//     if (pcb == NULL) {
//         printf("Out of memory for PCB");
//         return NULL;
//     }

//     // Allocate memory for the process stack
//     uint32_t* stack = (uint32_t*) kmalloc(STACK_SIZE);
//     if (stack == NULL) {
//         printf("Error: Out of memory for process stack\n");
//         return NULL;
//     }

//     // Map PCB and stack pages
//     memory_map_page((uint32_t)pcb, (uint32_t)pcb, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
//     memory_map_page((uint32_t)stack, (uint32_t)stack, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);

//     printf("stack base: %p\n", stack);

//     // Initialize the PCB fields
//     pcb->entry_point = entry_point;

//     // Set up the initial stack pointer (stack grows downward)
//     // pcb->stack = (uint32_t)((uint8_t*)stack + STACK_SIZE);  // Correct calculation
//     pcb->stack = &stack;
//     printf("stack top: %p\n", &pcb->stack);

//     // Initialize registers and other state
//     pcb->cpu_state.eax = 0;
//     pcb->cpu_state.ebx = 0;
//     pcb->cpu_state.ecx = 0;
//     pcb->cpu_state.edx = 0;
//     pcb->cpu_state.esi = 0;
//     pcb->cpu_state.edi = 0;
//     pcb->cpu_state.ebp = 0;

//     // Set up initial stack and entry point
//     pcb->cpu_state.esp = &pcb->stack;
//     pcb->cpu_state.eip = entry_point;

//     printf("cpu_state.esp: %p\n", pcb->cpu_state.esp);
//     printf("cpu_state.eip: %p\n", pcb->cpu_state.eip);
    
//     // Return the new process's PCB
//     return pcb;
// }







// Define constants or initial values for process state
#define STACK_SIZE 4096  // Stack size for each process
#define PAGE_DIR_SIZE 1024  // Assuming a page directory is 1024 entries
#define KERNEL_STACK_SIZE 4096 // Kernel stack size

// Create a new process
PCB* create_process(uint32_t entry_point) {
    // Allocate memory for the PCB
    PCB *pcb = (PCB*) kmalloc(sizeof(PCB));
    if (pcb == NULL) {
        printf("Error: Out of memory for PCB\n");
        return NULL;
    }

    // Assign a unique PID (could use queue_tail for simplicity)
    pcb->pid = queue_tail;
    queue_tail = (queue_tail + 1) % MAX_PROCESSES;
    printf("yoo\n");

    // Allocate memory for the process stack
    uint32_t *stack = (uint32_t*) kmalloc(STACK_SIZE);
    if (stack == NULL) {
        printf("Error: Out of memory for process stack\n");
        kfree(pcb); // Clean up PCB memory if stack allocation fails
        return NULL;
    }

    // Allocate memory for the page directory
    // pcb->page_dir = memory_alloc_page_dir();  // Custom function to allocate a page directory
    // if (pcb->page_dir == NULL) {
    //     printf("Error: Out of memory for page directory\n");
    //     kfree(stack);  // Clean up stack memory if page directory allocation fails
    //     kfree(pcb);    // Clean up PCB memory
    //     return NULL;
    // }

    // Initialize the PCB fields
    pcb->entry_point = entry_point;
    pcb->state = READY;  // Assume the process starts in the READY state

    // Initialize registers and other CPU state values to 0
    pcb->cpu_state.eax = 0;
    pcb->cpu_state.ebx = 0;
    pcb->cpu_state.ecx = 0;
    pcb->cpu_state.edx = 0;
    pcb->cpu_state.esi = 0;
    pcb->cpu_state.edi = 0;
    pcb->cpu_state.ebp = 0;

    // Set up initial stack and registers
    pcb->stack = stack;  // Set the stack pointer for the process
    pcb->cpu_state.esp = (uint32_t)(stack + STACK_SIZE / sizeof(uint32_t)); // Stack pointer to top of the stack
    pcb->cpu_state.eip = entry_point; // Entry point for the process

    // Add the new process to the process queue
    process_queue[queue_tail] = pcb;
    queue_tail = (queue_tail + 1) % MAX_PROCESSES;

    // Debug print statements to show the initialized values
    printf("PCB created for process %d\n", pcb->pid);
    printf("Entry Point: %p\n", (void*) pcb->entry_point);
    printf("Stack Pointer (ESP): %p\n", (void*) pcb->cpu_state.esp);
    printf("Stack Base: %p\n", (void*) pcb->stack);
    // printf("Page Directory: %p\n", (void*) pcb->page_dir);
    printf("Process State: %d\n", pcb->state);
    printf("CPU State: eax=%d, ebx=%d, ecx=%d, edx=%d, esi=%d, edi=%d, ebp=%d\n",
           pcb->cpu_state.eax, pcb->cpu_state.ebx, pcb->cpu_state.ecx,
           pcb->cpu_state.edx, pcb->cpu_state.esi, pcb->cpu_state.edi, pcb->cpu_state.ebp);

    return pcb;
}








// Destroy a process
void destroy_process(uint32_t pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_queue[i] != NULL && process_queue[i]->pid == pid) {
            kfree(process_queue[i]->stack);      // Free the stack memory
            kfree(process_queue[i]->page_dir);   // Free the page directory
            kfree(process_queue[i]);              // Free the PCB
            process_queue[i] = NULL;              // Remove from the queue
            break;
        }
    }
}


// Schedule the next process (Round Robin)
void schedule() {
    if (queue_head == queue_tail) return;  // No processes to schedule
    
    current_process_idx = queue_head;     // Select the first process in the queue
    queue_head = (queue_head + 1) % MAX_PROCESSES;
    
    context_switch(NULL, process_queue[current_process_idx]); // Perform a context switch
}


// Context switch (save the state of the old process and load the new process)
void context_switch(PCB *old_proc, PCB *new_proc) {
    
    if (new_proc == NULL) {
        printf("Failed to allocate memory for PCB");
    }

    printf("stack: %d\n");
    if (new_proc->stack == NULL) {
        printf("Failed to allocate memory for process stack");
    }
    
    if (old_proc != NULL) {
        // Save the state of the old process (e.g., registers, program counter)
        save_process_state(old_proc);
    }
    
    if (new_proc != NULL) {
        // Load the state of the new process (e.g., set the page directory, load registers)
        load_process_state(new_proc);
    }
}


// Exit the current process (cleanup)
void process_exit() {
    destroy_process(process_queue[current_process_idx]->pid);
    schedule(); // Schedule the next process
}


