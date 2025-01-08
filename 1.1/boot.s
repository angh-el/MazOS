; BITS 32



; section .text
;     ALIGN 4
;     DD 0x1BADB002
;     DD 0x00000003
;     DD -(0x1BADB002 + 0x00000003)

section .multiboot
ALIGN 4
    DD 0x1BADB002
    DD 0x00000003
    DD -(0x1BADB002 + 0x00000003)
    DD 0, 0, 0, 0, 0

    ; graphics mode settings
    DD 0
    DD 800
    DD 600
    DD 32

SECTION .bss
ALIGN 16
stack_bottom:
    RESB 16384 * 8
stack_top:

section .boot

global start
start:
    MOV ecx, (initial_page_directory - 0xC0000000)
    MOV cr3, ecx

    MOV ecx, cr4
    OR ecx, 0x10
    MOV cr4, ecx

    MOV ecx, cr0
    OR ecx, 0x80000000
    MOV cr0, ecx
 
    JMP higher_half


section .text
higher_half:
    MOV esp, stack_top
    PUSH ebx
    PUSH eax
    XOR ebp, ebp
    extern kmain
    CALL kmain

halt:
    hlt
    JMP halt


section .data
ALIGN 4096
global initial_page_directory
initial_page_directory:
    DD 10000011b
    TIMES 768-1 DD 0

    DD (0 << 22) | 10000011b
    DD (1 << 22) | 10000011b
    DD (2 << 22) | 10000011b
    DD (3 << 22) | 10000011b
    TIMES 256-4 DD 0
