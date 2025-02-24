; section .text
; global switch_to

; switch_to:
;     pusha                   ; Save general-purpose registers
;     mov  eax, [esp + 36]    ; Get pointer to old process struct
;     mov  [eax], esp         ; Save ESP of old process
;     mov  [eax + 4], ebp     ; Save EBP of old process
;     mov  ebx, cr3           ; Save CR3 into EBX first
;     mov  [eax + 8], ebx     ; Store CR3 in the old process struct

;     mov  eax, [esp + 40]    ; Get pointer to new process struct
;     mov  esp, [eax]         ; Load ESP of new process
;     mov  ebp, [eax + 4]     ; Load EBP of new process
;     mov  ebx, [eax + 8]     ; Load CR3 from new process struct
;     mov  cr3, ebx           ; Restore CR3

;     popa                    ; Restore registers
;     ret






; global switch_context
; switch_context:
;     ; Save old process context
;     ; pusha
;     mov  eax, [esp + 4]      ; Get pointer to old process struct
;     mov  [eax], esp          ; Save ESP of current process
;     mov  [eax + 4], ebp      ; Save EBP of current process
;     mov  ebx, cr3            ; Save current page directory
;     mov  [eax + 8], ebx      ; Store CR3 of current process

;     ; Load new process context
;     mov  eax, [esp + 8]      ; Get pointer to new process struct
;     mov  esp, [eax]          ; Load ESP of new process
;     mov  ebp, [eax + 4]      ; Load EBP of new process
;     mov  ebx, [eax + 8]      ; Load CR3 from new process struct
;     mov  cr3, ebx            ; Restore CR3

;     ; popa
;     ret                      ; Return to new process





; global switch_context

; switch_context:
;     cli                         ; Disable interrupts to avoid race conditions

;     pusha                       ; Save all general-purpose registers
;     pushf                       ; Save EFLAGS

;     mov  eax, [esp + 40]        ; Get pointer to old process struct (saved ESP + ret addr)
;     test eax, eax
;     jz   .skip_save             ; Skip saving if NULL (first process switch)

;     mov  [eax], esp             ; Save ESP of current process
;     mov  [eax + 4], ebp         ; Save EBP of current process
;     mov  ebx, cr3               ; Save current page directory
;     mov  [eax + 8], ebx         ; Store CR3 of current process

; .skip_save:
;     ; Load new process context
;     mov  eax, [esp + 44]        ; Get pointer to new process struct
;     test eax, eax
;     jz   .halt                  ; If NULL, halt system (avoid crash)

;     mov  esp, [eax]             ; Load ESP of new process
;     mov  ebp, [eax + 4]         ; Load EBP of new process
;     mov  ebx, [eax + 8]         ; Load CR3 from new process struct
;     mov  cr3, ebx               ; Restore CR3

;     popf                        ; Restore EFLAGS
;     popa                        ; Restore registers

;     sti                         ; Re-enable interrupts
;     ret                         ; Return to new process

; .halt:
;     hlt                         ; Halt CPU if switch fails
;     jmp .halt                   ; Prevent execution past this point




; global switch_context

; switch_context:
;     ; Save the context of the current process (old process)
;     ; pusha                       ; Save all general-purpose registers
;     ; pushf                       ; Save EFLAGS register

; ;     ; Save current process state
; ;     mov  eax, [esp + 40]        ; Get pointer to old process context (passed from C)
; ;     test eax, eax
; ;     jz   .skip_save             ; If NULL (first process), skip saving
; ;     mov  [eax], esp             ; Save ESP of the current process
; ;     mov  [eax + 4], ebp         ; Save EBP of the current process
; ;     mov  ebx, cr3               ; Save current page directory (CR3)
; ;     mov  [eax + 8], ebx         ; Store CR3 (page directory) of current process

; ; .skip_save:
; ;     ; Load the context of the new process
; ;     mov  eax, [esp + 44]        ; Get pointer to new process context (passed from C)
; ;     test eax, eax
; ;     jz   .halt                  ; If NULL, halt CPU to avoid invalid switch

; ;     mov  esp, [eax]             ; Load ESP of the new process
; ;     mov  ebp, [eax + 4]         ; Load EBP of the new process
; ;     mov  ebx, [eax + 8]         ; Load CR3 (page directory) of the new process
; ;     mov  cr3, ebx               ; Restore CR3 (page directory) for new process

; ;     popf                        ; Restore EFLAGS
; ;     popa                        ; Restore all general-purpose registers

; ;     ; sti                         ; Enable interrupts
;     ret                         ; Return to the new process (via its entry point)

; ; .halt:
; ;     hlt                         ; Halt the CPU if something went wrong


















; ; C declaration:
; ; void switch_to_task(task_control_block_t *next_task);

; global switch_to_task:
;     ; Save the state of the current task

;     push ebx                ; Save ebx
;     push esi                ; Save esi
;     push edi                ; Save edi
;     push ebp                ; Save ebp

;     ; Save the current task's kernel stack pointer (ESP)
;     mov edi, [current_task_TCB]      ; Load current task's TCB address into edi
;     mov [edi + TCB.ESP], esp         ; Save ESP for the current task in its TCB

;     ; Load the next task's state from the TCB (task control block)
;     mov esi, [esp + 4]                ; Get the address of the next task's TCB (parameter passed on stack)
;     mov [current_task_TCB], esi       ; Update the current task pointer to the next task's TCB

;     mov esp, [esi + TCB.ESP]          ; Load ESP for next task's kernel stack from its TCB
;     mov eax, [esi + TCB.CR3]          ; Load the CR3 (virtual address space) for the next task
;     mov ebx, [esi + TCB.ESP0]         ; Load the top of the next task's kernel stack (for TSS)
;     mov [TSS.ESP0], ebx               ; Update the TSS with the top of the new task's kernel stack
;     mov ecx, cr3                      ; Save the current task's CR3 (virtual address space)

;     ; Check if the virtual address space (CR3) has changed
;     cmp eax, ecx                      ; Compare current and next task's address spaces
;     je .doneVAS                       ; If the same, skip reloading CR3
;     mov cr3, eax                      ; If different, load the new address space (CR3)
; .doneVAS:

;     ; Restore the saved registers (restore the context of the current task)
;     pop ebp                ; Restore ebp
;     pop edi                ; Restore edi
;     pop esi                ; Restore esi
;     pop ebx                ; Restore ebx

;     ret                     ; Jump to the next task's EIP (loaded on its kernel stack)









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
