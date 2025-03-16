#ifndef SYSCALL_H
#define SYSCALL_H


// #include "stdint.h"

/* ===== Syscall Numbers ===== */
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

typedef unsigned long size_t;
typedef signed long ssize_t;
typedef int pid_t;



// Syscall handler
int syscall_handler(int syscall_num, ...);

// Syscall Function Prototypes
void sys_exit(int status);
ssize_t sys_write(int fd, const void *buf, size_t count);
ssize_t sys_read(int fd, void *buf, size_t count);
int sys_open(const char *pathname, int flags, int mode);
int sys_close(int fd);
void *sys_sbrk(int increment);
pid_t sys_fork(void);
int sys_execve(const char *path, char *const argv[], char *const envp[]);
pid_t sys_waitpid(pid_t pid, int *status, int options);
pid_t sys_getpid(void); 





#endif