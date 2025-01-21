#ifndef PROCESS_H
#define PROCESS_H

#include "kernel_memory.h"
#include "memory_init.h"
#include "stdint.h"

#define MAX_PROCESSES 64
#define KERNEL_STACK_SIZE 0x1000

// Process Control Block
typedef struct CPUState {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esi, edi, ebp, esp, eip, eflags;
} CPUState;

typedef enum {
    READY,       // Process is ready to run
    RUNNING,     // Process is currently running
    WAITING,     // Process is waiting for an event or I/O
    TERMINATED   // Process has finished execution
} ProcessState;

// PCB (Process Control Block) structure
typedef struct {
    uint32_t pid;            // Process ID
    uint32_t *stack;         // Pointer to the process's stack
    uint32_t *page_dir;      // Pointer to the process's page directory
    ProcessState state;          // Process state (running, ready, blocked)
    uint32_t entry_point;    // Program entry point (where it begins execution)
    CPUState cpu_state;
} PCB;

extern PCB *process_queue[MAX_PROCESSES];   // process queue for scheduling
extern int current_process_idx;             // index of the currently running process


PCB* create_process(uint32_t entry_point);
void destroy_process(uint32_t pid);
void schedule();
void context_switch(PCB *old_proc, PCB *new_proc);
void process_exit();
void init_processes();

// void save_process_state(PCB *process);
// void load_process_state(PCB *process);

#endif