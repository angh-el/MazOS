section .bss
    ; Define global variables for task control block and TSS
    current_task_TCB resd 1        ; Pointer to current task's TCB
    TCB_ESP resd 1                 ; Stack pointer (ESP) for task
    TCB_CR3 resd 1                 ; Virtual address space (CR3) for task
    TCB_ESP0 resd 1                ; Kernel stack pointer (ESP0) for task
    TSS_ESP0 resd 1                ; Top of kernel stack (for TSS)

section .text
    global switch_to_task
    extern kmalloc

switch_to_task:
    ; Save previous task's state
    push ebx
    push esi
    push edi
    push ebp

    mov edi, [current_task_TCB]     ; edi = address of the current task's TCB
    mov [edi+TCB_ESP], esp          ; Save ESP for the current task in TCB

    ; Load next task's state
    mov esi, [esp + 4]              ; esi = address of the next task's TCB
    mov [current_task_TCB], esi     ; Update current_task_TCB to next task's TCB

    mov esp, [esi + TCB_ESP]        ; Load ESP for next task from its TCB
    mov eax, [esi + TCB_CR3]        ; Load CR3 (virtual address space) for next task
    mov ebx, [esi + TCB_ESP0]       ; Load ESP0 for the next task's kernel stack
    mov [TSS_ESP0], ebx             ; Update the TSS.ESP0 field for privilege level switch
    mov ecx, cr3                    ; Save the current task's CR3 (virtual address space)

    cmp eax, ecx                    ; Check if address space needs to be changed
    je .doneVAS                     ; If same, skip the reload of CR3

    mov cr3, eax                    ; Load new task's CR3 (address space)

.doneVAS:
    pop ebp
    pop edi
    pop esi
    pop ebx

    ret                             ; Return to the new task (load next task's EIP)
