#ifndef SYSCALL_HPP
#define SYSCALL_HPP

#include "../lib/stdint.hpp"
#include "../lib/printf.hpp"
#include "../drivers/storage/fat32.hpp"

#include "../descriptors/idt.hpp"

#define SYS_EXIT        1
#define SYS_WRITE       2
#define SYS_READ        3
#define SYS_OPEN        4
#define SYS_CLOSE       5
#define SYS_SBRK        6
#define SYS_FORK        7
#define SYS_EXECVE      8
#define SYS_WAITPID     9
#define SYS_GETPID      10
#define SYS_KILL        11

extern "C" void syscall_entry();

class Syscall {
public:
    static void init();

    static void exit(int status);
    static ssize_t write(int fd, const void *buf, size_t count);
    static ssize_t read(int fd, void *buf, size_t count);
    static int open(const char *pathname, int flags, int mode);
    static int close(int fd);
    static void *sbrk(int increment);
    static pid_t fork();
    static int execve(const char *path, char *const argv[], char *const envp[]);
    static int kill(int pid, int sig);

    static void handler(struct interrupt_register *regs);

};




#endif