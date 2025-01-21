section .text
    global save_process_state
    global load_process_state

; Save process state (registers, flags, stack pointer, etc.)
save_process_state:
    ; Save general-purpose registers to the PCB (assuming the PCB pointer is in edi)
    mov dword [edi], eax        ; process->cpu_state.eax
    mov dword [edi+4], ebx      ; process->cpu_state.ebx
    mov dword [edi+8], ecx      ; process->cpu_state.ecx
    mov dword [edi+12], edx     ; process->cpu_state.edx
    mov dword [edi+16], esi     ; process->cpu_state.esi
    mov dword [edi+20], edi     ; process->cpu_state.edi
    mov dword [edi+24], ebp     ; process->cpu_state.ebp
    
    pushf                         ; Save EFLAGS onto the stack
    pop dword [edi+28]        ; process->cpu_state.eflags

    mov dword [edi+32], esp   ; process->cpu_state.esp

    ; Save EIP (by using a trampoline technique)
    call save_eip                 ; Call save_eip to get EIP
    ret

save_eip:
    call .next                    ; Call to a label to get the return address (EIP)
.next:
    pop eax                       ; Pop the EIP into eax
    mov dword [edi+36], eax   ; Save the EIP into process->cpu_state.eip
    ret

; Load process state (restoring registers, flags, stack pointer, etc.)
load_process_state:
    ; Load general-purpose registers from the PCB (assuming the PCB pointer is in edi)
    mov eax, dword [edi]        ; process->cpu_state.eax
    mov ebx, dword [edi+4]      ; process->cpu_state.ebx
    mov ecx, dword [edi+8]      ; process->cpu_state.ecx
    mov edx, dword [edi+12]     ; process->cpu_state.edx
    mov esi, dword [edi+16]     ; process->cpu_state.esi
    mov edi, dword [edi+20]     ; process->cpu_state.edi
    mov ebp, dword [edi+24]     ; process->cpu_state.ebp

    push dword [edi+28]         ; Load EFLAGS from the PCB
    popf                            ; Pop into EFLAGS

    mov esp, dword [edi+32]     ; process->cpu_state.esp
    jmp dword [edi+36]          ; Jump to the loaded EIP (process->cpu_state.eip)
