global syscall_entry
syscall_entry:
    pusha                       ; Save  registers
    mov eax, [esp+36]           ; Get syscall number
    extern syscall_handler
    call syscall_handler        ; Call syscall handler 
    popa                        ; Restore registers
    iret                        ; Return from interrupt
