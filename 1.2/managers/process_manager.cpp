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
        // uint32_t phys_addr = physical_memory_alloc_page_frame();
        uint32_t phys_addr = MemoryManager::alloc_page_frame();
        if (!phys_addr) {
            kfree(new_task);
            kfree(new_stack);
            return nullptr; // Handle memory allocation failure
        }
        MemoryManager::map_page((uint32_t)new_stack + i, phys_addr, PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE);
    }
    

    uint32_t *current_page_dir = MemoryManager::get_current_page_dir();
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
    new_task->next_task = nullptr;  // No next task yet

    new_task->pid = pid;
    new_task->time_remaining = exec_time;
    // new_task->arrival_time = getTicks();
    new_task->tickets = tickets;
    

    // Set up the kernel stack for the new task    
    // uint32_t* stack_ptr = new_stack + KERNEL_STACK_SIZE - 1;
    uint32_t* stack_ptr = (uint32_t*)((uint8_t*)new_stack + KERNEL_STACK_SIZE - sizeof(uint32_t));

    uint32_t stack_page = (uint32_t)stack_ptr & ~0xFFF;  // Get the start of the page
    uint32_t* stack_page_table = REC_PAGETABLE(stack_page >> 22);
    uint32_t stack_page_idx = (stack_page >> 12) & 0x3FF;
   
    uint32_t task_function_addr = (uint32_t)task_function;

    // MemoryManager::map_page((uint32_t)task_startup, (uint32_t)task_startup - KERNEL_START, PAGE_FLAG_PRESENT );
    // MemoryManager::map_page((uint32_t)task_function, (uint32_t)task_function - KERNEL_START, PAGE_FLAG_PRESENT);

    // *stack_ptr-- = (uint32_t)task_startup;  // Return to task_startup first
    // *stack_ptr-- = (uint32_t)task_function;  // After startup, jump to the real task function


    // Save the stack pointer in the task's TCB
    // new_task->kernel_stack_top = (void*)stack_ptr;

    // Set up the TCB and return it
    return new_task;
}












/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////


// void clear_area(int start_col, int start_row, int width, int height) {
//     for (int row = start_row; row < start_row + height; row++) {
//         for (int col = start_col; col < start_col + width; col++) {
//             print_char(' ', col, row, WHITE_ON_BLACK); // Clear each position
//         }
//     }
// }

// // Function to check if a number is prime
// boolean is_prime(unsigned int num){
//     if (num < 2) return false;
//     for (unsigned int i = 2; i * i <= num; i++) {
//         if (num % i == 0) return false;
//     }
//     return true;
// }




// void process_2(){
//     int mid_col = MAX_COLS / 2;
//     int mid_row = MAX_ROWS / 2;
//     // process 2 code:
//     static unsigned int prime_candidate = 2; // Start checking from 2
//     static unsigned int last_prime = 0; // Keep track of the last prime found
//     static unsigned int reset_limit = 100000; // Upper limit for resetting (you can adjust as needed)

//     // Clear the top-right quarter of the screen
//     // Position for "Process 2" title in the second quarter
//     print_at("Process 2", mid_col + mid_col / 2 - 4, mid_row / 2 - 4);

//     // Clear the area where prime numbers will be displayed (middle of the second quarter)
//     clear_area(mid_col + mid_col / 2 - 16, mid_row / 2, 16, 1);
//     // Find the next prime and display it
//     while (!is_prime(prime_candidate)) {
//         prime_candidate++;
//     }

//     // Print the prime number
//     set_cursor(get_screen_offset(mid_col + 16, mid_row / 2));
//     printf("Prime: %d", prime_candidate);
//     last_prime = prime_candidate;

//     // Increment to next candidate for next check
//     prime_candidate++;

//     // Reset when reaching a high prime
//     if (last_prime >= reset_limit) {
//         prime_candidate = 2; // Reset prime candidate to start from 2 again
//         last_prime = 0; // Reset the last prime
//     }

//     // sleep();
// }

// #define GREEN_ON_GREEN 0x22
// #define RED_ON_RED 0x44

// // Defining constants
// #define BALL_RADIUS 5    // The radius of the circle the ball moves along
// #define BALL_CENTER_X (MAX_COLS / 4)  // X-coordinate of the center of the third quarter
// #define BALL_CENTER_Y (MAX_ROWS * 0.75) // Y-coordinate of the center of the third quarter

// // Frame buffer size (24 frames for the ball's rotation)
// #define NUM_FRAMES 20

// // The ball's position in each frame (precomputed positions in a circle)
// int ball_frames[NUM_FRAMES][2] = {
//     { BALL_CENTER_X + 0, BALL_CENTER_Y - BALL_RADIUS },
//     { BALL_CENTER_X + 2, BALL_CENTER_Y - 4 },
//     { BALL_CENTER_X + 4, BALL_CENTER_Y - 3 },
//     { BALL_CENTER_X + 6, BALL_CENTER_Y - 2 },
//     { BALL_CENTER_X + 8, BALL_CENTER_Y - 1 },
//     { BALL_CENTER_X + 10, BALL_CENTER_Y + 0 },
//     { BALL_CENTER_X + 8, BALL_CENTER_Y + 1 },
//     { BALL_CENTER_X + 6, BALL_CENTER_Y + 1 },
//     { BALL_CENTER_X + 4, BALL_CENTER_Y + 2 },
//     { BALL_CENTER_X + 2, BALL_CENTER_Y + 2 },
//     { BALL_CENTER_X + 0, BALL_CENTER_Y + 3 },
//     { BALL_CENTER_X - 2, BALL_CENTER_Y + 3 },
//     { BALL_CENTER_X - 4, BALL_CENTER_Y + 4 },
//     { BALL_CENTER_X - 6, BALL_CENTER_Y + 4 },
//     { BALL_CENTER_X - 8, BALL_CENTER_Y + 2 },
//     { BALL_CENTER_X - 10, BALL_CENTER_Y + 0 },
//     { BALL_CENTER_X - 8, BALL_CENTER_Y - 1 },
//     { BALL_CENTER_X - 6, BALL_CENTER_Y - 2 },
//     { BALL_CENTER_X - 4, BALL_CENTER_Y - 3 },
//     { BALL_CENTER_X - 2, BALL_CENTER_Y - 4 },
//     // { BALL_CENTER_X + 0, BALL_CENTER_Y - 4 },
//     // { BALL_CENTER_X + 2, BALL_CENTER_Y - 3 },
//     // { BALL_CENTER_X + 4, BALL_CENTER_Y - 2 },
//     // { BALL_CENTER_X + 6, BALL_CENTER_Y - 1 }
// };



// void process_3() {
//     static int current_frame = 0;  // Keeps track of the current frame in the animation

//     // Coordinates for the third quarter of the screen
//     int mid_col = MAX_COLS / 2;
//     int mid_row = MAX_ROWS / 2;
    
//     static int background_drawn = 0;  // Flag to ensure we draw the background only once

//     // Draw the green background only once
//     if (background_drawn == 0) {
//         for (int row = mid_row; row < MAX_ROWS; row++) {
//             for (int col = 0; col < mid_col; col++) {
//                 print_char(' ', col, row, GREEN_ON_GREEN);  // Draw the background in green
//             }
//         }
//         background_drawn = 1;  // Set flag to indicate background is drawn
//     }

//     // Position for "Process 3" title in the third quarter
//     print_at("Process 3", mid_col / 2 - 4, mid_row + mid_row / 2 - 4);

//     // Previous frame position (for clearing)
//     static int prev_frame = -1;  // Use -1 to indicate no previous position at first

//     if (prev_frame >= 0) {
//         // Get the previous ball position from the frames
//         int prev_x = ball_frames[prev_frame][0];
//         int prev_y = ball_frames[prev_frame][1];
        
//         // Clear the previous ball position by printing a space (red ball erased)
//         print_char(' ', prev_x, prev_y, GREEN_ON_GREEN);  // Overwrite previous ball position with green
//     }

//     // Get the current ball position
//     int ball_x = ball_frames[current_frame][0];
//     int ball_y = ball_frames[current_frame][1];
    
//     // Draw the ball at the current frame position (red ball on green background)
//     print_char('O', ball_x, ball_y, RED_ON_RED);

//     // Update the frame counter to the next frame for the next call
//     prev_frame = current_frame;  // Store the current frame as the previous frame for next time
//     current_frame = (current_frame + 1) % NUM_FRAMES;  // Cycle through frames
    
//     // Simulate delay for animation speed (this could be adjusted)
//     for (volatile int i = 0; i < 50000000; i++) {
    
//         // Simple busy wait delay loop to simulate frame rate
//     }
// }


// int strlen(const char* str) {
//     int len = 0;
//     while (str[len] != '\0') {
//         len++;
//     }
//     return len;
// }

// void process_4() {
     
//     static int offset = 0;  // Start printing from the right side
//     static int scroll_started = 0;  // Flag to check if scrolling has started
//     const char* text = "This demonstrates how the OS handles multiple processes running at the same time... talk about a bit about something";
//     int text_length = strlen(text);
    
//     if((offset < MAX_COLS / 2)){set_cursor(get_screen_offset(MAX_COLS-offset, MAX_ROWS*0.75));}
//     else{set_cursor(get_screen_offset(MAX_COLS/2 + 1, MAX_ROWS*0.75));}


//     if(offset == text_length){
//         offset = 0;
//     }

//     if(offset < MAX_COLS / 2){
//         for (int i = 0; i < offset && text[i] != '\0'; i++) {
//             printf("%c", text[i]);
//         }

//         offset++;
//     }
//     else if(offset >= MAX_COLS / 2 ){
//         for(int i = offset; i < offset + MAX_COLS / 2 && text[i] != '\0'; i++){
//             if(text[i] == '\0'){break;}
//             printf("%c", text[i]);
//         }
//         offset++;
//     }


//     for (volatile int i = 0; i < 50000000; i++) {
//         // Simple busy wait delay loop to simulate frame rate
//     }
// }




// void process_1() {
//     // // process 1 code
//     // static int counter = 0; // Use a static variable to persist the count between calls
//     // // Clear the top-left quarter of the screen
//     // int mid_col = MAX_COLS / 2;
//     // int mid_row = MAX_ROWS / 2;
//     // // Position for "Process 1" title in the first quarter
//     // print_at("Process 1", mid_col / 2 - 4, mid_row / 2 - 4);
//     // // Clear the area where "Hello World n" will appear (middle of the first quarter)
//     // clear_area(mid_col / 4 - 16, mid_row / 2, 16, 1); // Adjust 16 to fit the string length
//     // // Print the "Hello World n" with the counter value incremented
//     // printf("Hello World %d", counter);
//     // // Increment the counter for the next print
//     // counter++;

//     // // sleep();
// /////////////////////////////////////////////////////////////////////////////

//     // process 1 code
//     static int counter = 0; // Use a static variable to persist the count between calls
//     // Clear the top-left quarter of the screen
//     int mid_col = MAX_COLS / 2;
//     int mid_row = MAX_ROWS / 2;
//     // Position for "Process 1" title in the first quarter
//     print_at("Process 1", mid_col / 2 - 4, mid_row / 2 - 4);
//     // Clear the area where "Hello World n" will appear (middle of the first quarter)
//     clear_area(mid_col / 4 - 16, mid_row / 2, 16, 1); // Adjust 16 to fit the string length
//     // Print the "Hello World n" with the counter value incremented
//     printf("Hello World %d", counter);
//     // Increment the counter for the next print
//     counter++;

//     // sleep();


//     //////////////
//     mid_col = MAX_COLS / 2;
//     mid_row = MAX_ROWS / 2;
//     // process 2 code:
//     static unsigned int prime_candidate = 2; // Start checking from 2
//     static unsigned int last_prime = 0; // Keep track of the last prime found
//     static unsigned int reset_limit = 100000; // Upper limit for resetting (you can adjust as needed)

//     // Clear the top-right quarter of the screen
//     // Position for "Process 2" title in the second quarter
//     print_at("Process 2", mid_col + mid_col / 2 - 4, mid_row / 2 - 4);

//     // Clear the area where prime numbers will be displayed (middle of the second quarter)
//     clear_area(mid_col + mid_col / 2 - 16, mid_row / 2, 16, 1);
//     // Find the next prime and display it
//     while (!is_prime(prime_candidate)) {
//         prime_candidate++;
//     }

//     // Print the prime number
//     set_cursor(get_screen_offset(mid_col + 16, mid_row / 2));
//     printf("Prime: %d", prime_candidate);
//     last_prime = prime_candidate;

//     // Increment to next candidate for next check
//     prime_candidate++;

//     // Reset when reaching a high prime
//     if (last_prime >= reset_limit) {
//         prime_candidate = 2; // Reset prime candidate to start from 2 again
//         last_prime = 0; // Reset the last prime
//     }

//     /////////////////////////////////////
//     static int current_frame = 0;  // Keeps track of the current frame in the animation

//     // Coordinates for the third quarter of the screen
//     mid_col = MAX_COLS / 2;
//     mid_row = MAX_ROWS / 2;
    
//     static int background_drawn = 0;  // Flag to ensure we draw the background only once

//     // Draw the green background only once
//     if (background_drawn == 0) {
//         for (int row = mid_row; row < MAX_ROWS; row++) {
//             for (int col = 0; col < mid_col; col++) {
//                 print_char(' ', col, row, GREEN_ON_GREEN);  // Draw the background in green
//             }
//         }
//         background_drawn = 1;  // Set flag to indicate background is drawn
//     }

//     // Position for "Process 3" title in the third quarter
//     print_at("Process 3", mid_col / 2 - 4, mid_row + mid_row / 2 - 4);

//     // Previous frame position (for clearing)
//     static int prev_frame = -1;  // Use -1 to indicate no previous position at first

//     if (prev_frame >= 0) {
//         // Get the previous ball position from the frames
//         int prev_x = ball_frames[prev_frame][0];
//         int prev_y = ball_frames[prev_frame][1];
        
//         // Clear the previous ball position by printing a space (red ball erased)
//         print_char(' ', prev_x, prev_y, GREEN_ON_GREEN);  // Overwrite previous ball position with green
//     }

//     // Get the current ball position
//     int ball_x = ball_frames[current_frame][0];
//     int ball_y = ball_frames[current_frame][1];
    
//     // Draw the ball at the current frame position (red ball on green background)
//     print_char('O', ball_x, ball_y, RED_ON_RED);

//     // Update the frame counter to the next frame for the next call
//     prev_frame = current_frame;  // Store the current frame as the previous frame for next time
//     current_frame = (current_frame + 1) % NUM_FRAMES;  // Cycle through frames
    
//     // Simulate delay for animation speed (this could be adjusted)
//     for (volatile int i = 0; i < 50000000; i++) {
    
//         // Simple busy wait delay loop to simulate frame rate
//     }
//     ///////////////////////////////////////////
//     static int offset = 0;  // Start printing from the right side
//     static int scroll_started = 0;  // Flag to check if scrolling has started
//     const char* text = "This demonstrates how the OS handles multiple processes running at the same time... talk about a bit about something";
//     int text_length = strlen(text);
    
//     if((offset < MAX_COLS / 2)){set_cursor(get_screen_offset(MAX_COLS-offset, MAX_ROWS*0.75));}
//     else{set_cursor(get_screen_offset(MAX_COLS/2 + 1, MAX_ROWS*0.75));}


//     if(offset == text_length){
//         offset = 0;
//     }

//     if(offset < MAX_COLS / 2){
//         for (int i = 0; i < offset && text[i] != '\0'; i++) {
//             printf("%c", text[i]);
//         }

//         offset++;
//     }
//     else if(offset >= MAX_COLS / 2 ){
//         for(int i = offset; i < offset + MAX_COLS / 2 && text[i] != '\0'; i++){
//             if(text[i] == '\0'){break;}
//             printf("%c", text[i]);
//         }
//         offset++;
//     }


//     for (volatile int i = 0; i < 5000000; i++) {
//         // Simple busy wait delay loop to simulate frame rate
//     }

// }






// void draw_quarters() {
//     int mid_col = MAX_COLS / 2;
//     int mid_row = MAX_ROWS / 2;

//     // Print labels in each quarter
//     print_at("Process 1", mid_col / 2 - 4, mid_row / 2);
//     print_at("Process 2", mid_col + mid_col / 2 - 4, mid_row / 2);
//     // print_at("Process 3", mid_col / 2 - 4, mid_row + mid_row / 2);
//     print_at("Process 4", mid_col + mid_col / 2 - 4, mid_row + mid_row / 2);

//     // Draw horizontal divider
//     for (int col = 0; col < MAX_COLS; col++) {
//         print_char('-', col, mid_row, WHITE_ON_BLACK);
//     }

//     // Draw vertical divider
//     for (int row = 0; row < MAX_ROWS; row++) {
//         print_char('|', mid_col, row, WHITE_ON_BLACK);
//     }
// }










/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////
/*
ALGORITHM TO EVALUATE WHICH SCHEDULING ALGORITHM IS MOST EFFICIENT
*/
void init_tasks() {
    // for (int i = 0; i < 4; i++) {
        task_list[0] = create_kernel_task( &process_1, 0, (0 + 1) * 10, (0 + 1) * 5); 
        task_list[1] = create_kernel_task(&process_1, 1, (1 + 1) * 10, (1 + 1) * 5); 
        task_list[2] = create_kernel_task( &process_1, 2, (2 + 1) * 10, (2 + 1) * 5); 
        task_list[3] = create_kernel_task( &process_1, 3, (3 + 1) * 10, (3 + 1) * 5); 
    // }
    current_task = task_list[0];
}


void schedule_RR() {
    if (current_task->time_remaining > 0) {
        current_task->time_remaining -= TIME_QUANTUM;
    }
    if (current_task->time_remaining <= 0) {
        current_task->state = 2; // Mark as finished
    }
    current_task_index = (current_task_index + 1) % NUM_TASKS;
    current_task = task_list[current_task_index];
}


void schedule_FCFS() {
    for (int i = 0; i < NUM_TASKS; i++) {
        if (task_list[i]->state == 0) {
            current_task = task_list[i];
            current_task->state = 1;
            while (current_task->time_remaining > 0) {
                current_task->time_remaining--;
            }
            current_task->state = 2; // Mark as finished
        }
    }
}


void schedule_Lottery() {
    int total_tickets = 0;
    for (int i = 0; i < NUM_TASKS; i++) {
        if (task_list[i]->state == 0) {
            total_tickets += task_list[i]->tickets;
        }
    }
    int winner = rand() % total_tickets;
    int sum = 0;
    for (int i = 0; i < NUM_TASKS; i++) {
        if (task_list[i]->state == 0) {
            sum += task_list[i]->tickets;
            if (sum > winner) {
                current_task = task_list[i];
                current_task->state = 1;
                current_task->time_remaining--;
                if (current_task->time_remaining <= 0) {
                    current_task->state = 2;
                }
                break;
            }
        }
    }
}


// Scheduler Runner
void run_scheduler(const char* algorithm) {
    printf("Running %s Scheduling\n", algorithm);
    selected_algorithm = algorithm;
    init_tasks();

    while (1) {
        int finished_tasks = 0;
        for (int i = 0; i < NUM_TASKS; i++) {
            if (task_list[i]->state == 2) {
                finished_tasks++;
            }
        }
        if (finished_tasks == NUM_TASKS) {
            break;
        }

        if (strncmp(algorithm, "RR", 2) == 0) {
            schedule_RR();
        } else if (strncmp(algorithm, "FCFS", 4) == 0) {
            schedule_FCFS();
        } else if (strncmp(algorithm, "Lottery", 7) == 0) {
            schedule_Lottery();
        }
    }

    // Calculate metrics
    int total_turnaround_time = 0;
    int total_response_time = 0;

    printf("\nTask Metrics for %s Scheduling:\n", algorithm);
    for (int i = 0; i < NUM_TASKS; i++) {
        int turnaround_time = task_list[i]->completion_time - task_list[i]->arrival_time;
        int response_time = task_list[i]->start_time - task_list[i]->arrival_time;
        total_turnaround_time += turnaround_time;
        total_response_time += response_time;
        printf("Task %d - Turnaround Time: %d, Response Time: %d\n", task_list[i]->pid, turnaround_time, response_time);
    }

    int avg_turnaround_time = total_turnaround_time / (int)NUM_TASKS;
    int avg_response_time = total_response_time / (int)NUM_TASKS;

    printf("Average Turnaround Time: %d\n", avg_turnaround_time);
    printf("Average Response Time: %d\n", avg_response_time);
    printf("%s Scheduling Completed.\n\n", algorithm);
}



void schedule() {
    if (strncmp(selected_algorithm, "RR",2) == 0) {
        schedule_RR();
    } else if (strncmp(selected_algorithm, "FCFS", 4) == 0) {
        schedule_FCFS();
    } else if (strncmp(selected_algorithm, "Lottery", 7) == 0) {
        schedule_Lottery();
    }
}


void schedule_test() {
    run_scheduler("RR");
    run_scheduler("FCFS");
    run_scheduler("Lottery");
}
//////////////////////////////////////////////////////////////