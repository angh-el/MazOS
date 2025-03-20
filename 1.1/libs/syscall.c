#include "syscall.h"
#include "../uefi/uefi.h"  



// extern efi_system_table_t *ST;
// // extern EFI_BOOT_SERVICES *BS;

efi_system_table_t *ST = 0x5000;
efi_boot_services_t *BS = 0x1000;


ssize_t sys_write(int fd, const void *buf, size_t count) {
    if (fd != 1) return -1;  

    uint16_t uefi_buf[256];
    int len = (count < 256) ? count : 255;

    for (int i = 0; i < len; i++) {
        uefi_buf[i] = ((char *)buf)[i];
    }
    uefi_buf[len] = 0;  

    ST->ConOut->OutputString(ST->ConOut, uefi_buf);
    return count;
}

ssize_t sys_read(int fd, void *buf, size_t count) {
    if (fd != 0) return -1; 

    // EFI_INPUT_KEY key;
    efi_input_key_t key;
    // char key;
    size_t i = 0;
    while (i < count) {
        while (ST->ConIn->ReadKeyStroke(ST->ConIn, &key) != EFI_SUCCESS);
        ((char *)buf)[i++] = key.UnicodeChar;
        if (key.UnicodeChar == '\n') break; 
    }
    return i;
}


void *sys_sbrk(int increment) {
    static char *heap_end = NULL;
    if (!heap_end) heap_end = (char *)0x100000; 

    char *old_end = heap_end;
    heap_end += increment;
    return old_end;
}

void sys_exit(int status) {
    printf("Process exited with status %d\n", status);
     
    for(;;); // halt 
}


#define False 0

int sys_execve(const char *path, char *const argv[], char *const envp[]) {
    efi_status_t status;
    efi_handle_t image_handle;
    efi_loaded_image_protocol_t *loaded_image;

    // Load  application
    status = BS->LoadImage(False, path, NULL, 0, &image_handle, 0);
    if (EFI_ERROR(status)) return -1;

    // Start application
    status = BS->StartImage(image_handle, NULL, NULL);
    return (EFI_ERROR(status)) ? -1 : 0;
}



#define MAX_FILES 16  //max open files
FILE *file_table[MAX_FILES] = { NULL };  // file descriptor table
int sys_open(const char *pathname, int flags, int mode) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i] == NULL) {  
            // FILE *file = fopen(pathname, "r");
            FILE *file = read_file(pathname);
            if (!file) return -1;  

            file_table[i] = file;
            return i;  
        }
    }
    return -1;  
}


int sys_close(int fd) {
    if (fd < 0 || fd >= MAX_FILES || file_table[fd] == NULL) {
        return -1;  // invalid FD
    }
    
    // fclose(file_table[fd]);  
    file_table[fd] = NULL;  // mark as closed
    return 0;
}


typedef struct {
    int pid;
    char *memory;
    int active;
} process_t;

#define MAX_PROCESSES 32
process_t process_table[MAX_PROCESSES] = {0};
int next_pid = 1;

int sys_fork() {
    int parent_pid = next_pid - 1;

    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].active == 0) {
            process_table[i].pid = next_pid++;
            process_table[i].active = 1;

            process_table[i].memory = process_table[parent_pid].memory;

            return process_table[i].pid;
        }
    }
    return -1;  
}

int sys_kill(int pid, int sig) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].active = 0;
            return 0;  
        }
    }
    return -1;  
}





/*
    El Handler
*/
int syscall_handler(int syscall_num, ...) {
    va_list args;
    va_start(args, syscall_num);
    
    switch (syscall_num) {
        case SYS_WRITE: {
            int fd = va_arg(args, int);
            const void *buf = va_arg(args, const void *);
            size_t count = va_arg(args, size_t);
            va_end(args);
            return sys_write(fd, buf, count);
        }
        case SYS_EXIT: {
            int status = va_arg(args, int);
            va_end(args);
            sys_exit(status);
            break;
        }
        case SYS_SBRK: {
            int increment = va_arg(args, int);
            va_end(args);
            return (int)sys_sbrk(increment);
        }
        case SYS_READ: {
            int fd = va_arg(args, int);
            void *buf = va_arg(args, void *);
            size_t count = va_arg(args, size_t);
            va_end(args);
            return sys_read(fd, buf, count);
        }
        case SYS_OPEN: {
            const char *pathname = va_arg(args, const char *);
            int flags = va_arg(args, int);
            int mode = va_arg(args, int);
            va_end(args);
            return sys_open(pathname, flags, mode);
        }
        case SYS_CLOSE: {
            int fd = va_arg(args, int);
            va_end(args);
            return sys_close(fd);
        }
        case SYS_EXECVE: {
            const char *path = va_arg(args, const char *);
            char *const *argv = va_arg(args, char *const *);
            char *const *envp = va_arg(args, char *const *);
            va_end(args);
            return sys_execve(path, argv, envp);
        }
        case SYS_FORK: {
            va_end(args);
            return sys_fork();
        }
        case SYS_KILL: {
            int pid = va_arg(args, int);
            int sig = va_arg(args, int);
            va_end(args);
            return sys_kill(pid, sig);
        }
        default:
            va_end(args);
            return -1;  // Invalid syscall
    }
    return 0;
}


void init_syscalls() {
    irq_install_handler(128 - 32, syscall_handler);  
}