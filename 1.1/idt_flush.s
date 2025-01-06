global idt_flush
idt_flush:
    MOV eax, [esp + 4]
    LIDT [eax]
    STI
    RET

%macro no_error_code_interrupt_handler 1
    global isr%1
    isr%1:
        CLI
        PUSH LONG 0                 ; push 0 as error code
        PUSH LONG %1                ; push interrupt number
        JMP common_interrupt_handler; jump to common handler
%endmacro

%macro error_code_interrupt_handler 1
    global isr%1
    isr%1:
        CLI
        PUSH LONG %1                ; push interrupt numbet
        JMP common_interrupt_handler; jumt to common handler
%endmacro


%macro interrupt_request 2
    global irq%1
    irq%1:
        CLI
        PUSH LONG 0
        PUSH LONG %2
        JMP common_interrupt_request_handler
%endmacro




no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler    8
no_error_code_interrupt_handler 9
error_code_interrupt_handler    10
error_code_interrupt_handler    11
error_code_interrupt_handler    12
error_code_interrupt_handler    13
error_code_interrupt_handler    14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
no_error_code_interrupt_handler 17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31
no_error_code_interrupt_handler 128
no_error_code_interrupt_handler 177


interrupt_request   0,    32
interrupt_request   1,    33
interrupt_request   2,    34
interrupt_request   3,    35
interrupt_request   4,    36
interrupt_request   5,    37
interrupt_request   6,    38
interrupt_request   7,    39
interrupt_request   8,    40
interrupt_request   9,    41
interrupt_request  10,    42
interrupt_request  11,    43
interrupt_request  12,    44
interrupt_request  13,    45
interrupt_request  14,    46
interrupt_request  15,    47




extern handle_isr
common_interrupt_handler:
    pusha
    mov eax,ds
    PUSH eax
    MOV eax, cr2
    PUSH eax
    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax
    PUSH esp
    CALL handle_isr
    ADD esp, 8
    POP ebx
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx
    POPA
    ADD esp, 8
    STI
    IRET   

extern handle_irq
common_interrupt_request_handler:
    pusha
    mov eax,ds
    PUSH eax
    MOV eax, cr2
    PUSH eax

    MOV ax, 0x10
    MOV ds, ax
    MOV es, ax
    MOV fs, ax
    MOV gs, ax

    PUSH esp
    CALL handle_irq

    ADD esp, 8
    POP ebx
    MOV ds, bx
    MOV es, bx
    MOV fs, bx
    MOV gs, bx
    
    POPA
    ADD esp, 8
    STI
    IRET