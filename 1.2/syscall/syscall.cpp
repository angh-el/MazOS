#include "syscall.hpp"
#include "./uefi/uefi.h"

extern efi_system_table_t *ST;
extern efi_boot_services_t *BS;

efi_system_table_t *ST = (efi_system_table_t *)0x5000;
efi_boot_services_t *BS = (efi_boot_services_t *)0x1000;

ssize_t Syscall::write(int fd, const void *buf, size_t count) {
    if (fd != 1) return -1;
    uint16_t uefi_buf[256];
    size_t len = (count < 256) ? count : 255;

    for (size_t i = 0; i < len; i++)
        uefi_buf[i] = ((char *)buf)[i];
    
    uefi_buf[len] = 0;
    ST->ConOut->OutputString(ST->ConOut, (wchar_t *)uefi_buf);
    return count;
}

ssize_t Syscall::read(int fd, void *buf, size_t count) {
    if (fd != 0) return -1;
    efi_input_key_t key;
    size_t i = 0;

    while (i < count) {
        while (ST->ConIn->ReadKeyStroke(ST->ConIn, &key) != EFI_SUCCESS);
        ((char *)buf)[i++] = key.UnicodeChar;
        if (key.UnicodeChar == '\n') break;
    }

    return i;
}

void *Syscall::sbrk(int increment) {
    static char *heap_end = nullptr;
    if (!heap_end) heap_end = (char *)0x100000;
    char *old_end = heap_end;
    heap_end += increment;
    return old_end;
}

void Syscall::exit(int status) {
    printf("Process exited with status %d\n", status);
    for (;;);
}

int Syscall::execve(const char *path, char *const argv[], char *const envp[]) {
    efi_status_t status;
    efi_handle_t image_handle;

    // status = BS->LoadImage(0, (void *)path, nullptr, 0, (uint8_t)&image_handle, 0);
    if (EFI_ERROR(status)) return -1;

    status = BS->StartImage(image_handle, nullptr, nullptr);
    return EFI_ERROR(status) ? -1 : 0;
}

#define MAX_FILES 16
static FILE *file_table[MAX_FILES] = {nullptr};

int Syscall::open(const char *pathname, int flags, int mode) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i]) {
            // BUG NEED TO FIX
            // I AM INVESTIGATING IT

            Fat32 fat32;
            // FILE *file = (FILE *)fat32.read_file(pathname);
            // if (!file) return -1;
            // file_table[i] = file;
            return i;
        }
    }
    return -1;
}

int Syscall::close(int fd) {
    if (fd < 0 || fd >= MAX_FILES || !file_table[fd])
        return -1;

    file_table[fd] = nullptr;
    return 0;
}

typedef struct {
    int pid;
    char *memory;
    int active;
} process_t;

#define MAX_PROCESSES 32
static process_t process_table[MAX_PROCESSES] = {};
static int next_pid = 1;

pid_t Syscall::fork() {
    int parent_pid = next_pid - 1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (!process_table[i].active) {
            process_table[i].pid = next_pid++;
            process_table[i].active = 1;
            process_table[i].memory = process_table[parent_pid].memory;
            return process_table[i].pid;
        }
    }
    return -1;
}

int Syscall::kill(int pid, int sig) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].active = 0;
            return 0;
        }
    }
    return -1;
}

void Syscall::handler(struct interrupt_register *regs) {
    va_list args;
    // va_start(args, syscall_num);
    int ret = 0;

    int syscall_num = regs->eax;
    switch (syscall_num) {
        case SYS_WRITE: {
            int fd = va_arg(args, int);
            const void *buf = va_arg(args, const void *);
            size_t count = va_arg(args, size_t);
            ret = write(fd, buf, count);
            break;
        }
        case SYS_EXIT: {
            int status = va_arg(args, int);
            exit(status);
            break;
        }
        case SYS_SBRK: {
            int increment = va_arg(args, int);
            ret = (int)sbrk(increment);
            break;
        }
        case SYS_READ: {
            int fd = va_arg(args, int);
            void *buf = va_arg(args, void *);
            size_t count = va_arg(args, size_t);
            ret = read(fd, buf, count);
            break;
        }
        case SYS_OPEN: {
            const char *pathname = va_arg(args, const char *);
            int flags = va_arg(args, int);
            int mode = va_arg(args, int);
            ret = open(pathname, flags, mode);
            break;
        }
        case SYS_CLOSE: {
            int fd = va_arg(args, int);
            ret = close(fd);
            break;
        }
        case SYS_EXECVE: {
            const char *path = va_arg(args, const char *);
            char *const *argv = va_arg(args, char *const *);
            char *const *envp = va_arg(args, char *const *);
            ret = execve(path, argv, envp);
            break;
        }
        case SYS_FORK:
            ret = fork();
            break;
        case SYS_KILL: {
            int pid = va_arg(args, int);
            int sig = va_arg(args, int);
            ret = kill(pid, sig);
            break;
        }
        default:
            ret = -1;
    }

    va_end(args);
    // return ret;
}

extern "C" void irq_install_handler(int irq, void (*handler)(void)); 
extern "C" int syscall_handler(int syscall_num, ...); 

int syscall_handler(int syscall_num, ...){
    va_list args;
    va_start(args, syscall_num);
    struct interrupt_register *regs;
    Syscall::handler(regs);

    return 1;
}


void Syscall::init() {
    // irq_install_handler(128 - 32, [] {
    //     asm volatile (
    //         "pusha\n"
    //         "movl 36(%%esp), %%eax\n"
    //         "call _ZN7Syscall7handlerEi\n"
    //         "popa\n"
    //         "iret\n"
    //     );
    // });
    printf("Syscalls succesfully initialised\n");
    DescriptorTables::IDT::irq_install_handler(128 - 32, handler);
}