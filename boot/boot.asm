
; ; PRINTS ALPHABET

; mov ah, 0x0e
; mov al, 'A' -1        

; loop:
;     inc al
;     cmp al, 'Z' +1
;     je exit
;     int 0x10
;     jmp loop

; exit:
;     jmp $

; times 510-($-$$) db 0
; ; db 0x55, 0xaa
; dw 0xaa55



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; [org 0x7c00]

; main:
;     ; NEW LINE
;     mov ah, 0x0e
;     mov al, 13
;     int 0x10
;     mov al, 10
;     int 0x10

;     mov ah, 0x0e
;     mov bx, Maz_OS_message
;     jmp print_welcome_message

;     get_input:
;         ; NEW LINE
;         mov ah, 0x0e
;         mov al, 13
;         int 0x10
;         mov al, 10
;         int 0x10

;         mov bx, user_input_variable
;         input_loop:
;             mov ah, 0
;             int 0x16
;             cmp al, 0x0D     ; to escape when enter is hit
;             je print_user_input
;             mov [bx], al
;             inc bx

;             ; mov ax, 0
;             ; int 0x16
;             mov ah, 0x0e
;             int 0x10
    
;             jmp input_loop

;     jmp end

; print_welcome_message:
;     mov al, [bx]
;     cmp al, 0
;     je get_input
;     int 0x10
;     inc bx
;     jmp print_welcome_message

; print_user_input:

;     ; NEW LINE
;         mov ah, 0x0e
;         mov al, 13
;         int 0x10
;         mov al, 10
;         int 0x10

;     mov ah, 0x0e
;     mov bx, user_input_variable 
;     variable_loop:

;         mov al, [bx]
;         cmp al, 0
;         ; je end
;         je get_input
;         int 0x10
;         inc bx
;         jmp variable_loop


; end:
;     jmp $

; Maz_OS_message:
;     ; db "", 0x0D, 0x0A
;     db "##     ##    ###    ########     #######   ######  ", 0x0D, 0x0A
;     db "###   ###   ## ##        ##     ##     ## ##       ", 0x0D, 0x0A
;     db "#### ####  ##   ##      ##      ##     ## ##       ", 0x0D, 0x0A
;     db "## ### ## ##     ##    ##       ##     ##  ######  ", 0x0D, 0x0A
;     db "##     ## #########   ##        ##     ##       ## ", 0x0D, 0x0A
;     db "##     ## ##     ##  ##         ##     ## ##    ## ", 0x0D, 0x0A
;     db "##     ## ##     ## ########     #######   ######  ", 0x0D, 0x0A
;     db 0

; user_input_variable:
;     times 10 db 0

; times 510-($-$$) db 0
; dw 0xaa55



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; ;
; ; A boot sector that prints a string using our function.
; ;

; [ org 0x7c00 ]          ; Tell the assembler where this code will be loaded


; mov bx , HELLO_MSG      ; Use BX as a parameter to our function , so
; call print_function       ; we can specify the address of a string.

; mov bx , GOODBYE_MSG
; call print_function

; mov bx , Maz_OS_message
; call print_function

; jmp $                   ; Hang

; %include "print_function.asm"

; ; Data
; HELLO_MSG :
; db ' Hello , World ! ', 0x0D, 0x0A, 0  ; <-- The zero on the end tells our routine  
;                             ; when to stop printing characters.

; GOODBYE_MSG :
; db ' Goodbye ! ', 0x0D, 0x0A, 0

; Maz_OS_message:
;     db "", 0x0D, 0x0A
;     db "##     ##    ###    ########     #######   ######  ", 0x0D, 0x0A
;     db "###   ###   ## ##        ##     ##     ## ##       ", 0x0D, 0x0A
;     db "#### ####  ##   ##      ##      ##     ## ##       ", 0x0D, 0x0A
;     db "## ### ## ##     ##    ##       ##     ##  ######  ", 0x0D, 0x0A
;     db "##     ## #########   ##        ##     ##       ## ", 0x0D, 0x0A
;     db "##     ## ##     ##  ##         ##     ## ##    ## ", 0x0D, 0x0A
;     db "##     ## ##     ## ########     #######   ######  ", 0x0D, 0x0A
;     db 0



; ; Padding and magic number.
; times 510 -( $ - $$ ) db 0
; dw 0xaa55





; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; ; A boot sector that enters 32 - bit protected mode.
; [org 0x7c00]

;     mov bp , 0x9000             ; Set the stack.
;     mov sp , bp
;     ; mov bx , MSG_REAL_MODE
;     ; call print_function
;     call switch_to_pm           ; Note that we never return from here.
;     jmp $

; ; %include "print_function.asm"
; %include "gdt.asm"
; %include "print_string_pm.asm"
; %include "switch_to_pm.asm"
; [bits 32]

; ; This is where we arrive after switching to and initialising protected mode.
; BEGIN_PM:
;     mov ebx , MSG_PROT_MODE
;     call print_string_pm        ; Use our 32 - bit print routine.
;     jmp $                       ; Hang.
; ; Global variables
; ; MSG_REAL_MODE: 
; ;     db "Started in 16 - bit Real Mode" , 0
; MSG_PROT_MODE: 
;     ; db "", 0x0D, 0x0A
;     db "##     ##    ###    ########     #######   ######  ", 0x0D, 0x0A
;     db "###   ###   ## ##        ##     ##     ## ##       ", 0x0D, 0x0A
;     db "#### ####  ##   ##      ##      ##     ## ##       ", 0x0D, 0x0A
;     db "## ### ## ##     ##    ##       ##     ##  ######  ", 0x0D, 0x0A
;     db "##     ## #########   ##        ##     ##       ## ", 0x0D, 0x0A
;     db "##     ## ##     ##  ##         ##     ## ##    ## ", 0x0D, 0x0A
;     db "##     ## ##     ## ########     #######   ######  ", 0x0D, 0x0A
;     db 0


; ; Bootsector padding
; times 510 -( $ - $$ ) db 0
; dw 0xaa55







; ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


; A boot sector that boots a C kernel in 32 - bit protected mode
[org 0x7c00]

KERNEL_OFFSET equ 0x1000            ; This is the memory offset to which we will load our kernel

    mov [BOOT_DRIVE], dl          ; BIOS stores our boot drive in DL , so it ’s
                                    ; best to remember this for later.
    
    mov bp , 0x9000                 ; Set -up the stack.
    mov sp , bp


    mov bx , MSG_REAL_MODE          ; Announce that we are starting
    call print_function               ; booting from 16 - bit real mode
    call load_kernel                ; Load our kernel
    call switch_to_pm               ; Switch to protected mode , from which
                                    ; we will not return
    jmp $


; Include our useful , hard - earned routines
%include "boot/print_function.asm"
%include "boot/disk_load.asm"
%include "boot/gdt.asm"
%include "boot/print_string_pm.asm"
%include "boot/switch_to_pm.asm"

[bits 16]
; load_kernel
load_kernel:

    mov bx , MSG_LOAD_KERNEL        ; Print a message to say we are loading the kernel
    call print_function
    mov bx , KERNEL_OFFSET          ; Set -up parameters for our disk_load routine , so
    mov dh , 50                     ; that we load the first 15 sectors ( excluding
    mov dl , [BOOT_DRIVE]         ; the boot sector ) from the boot disk ( i.e. our
    call disk_load                  ; kernel code ) to address KERNEL_OFFSET
    ret

[bits 32]
; This is where we arrive after switching to and initialising protected mode.

BEGIN_PM:
    mov ebx , MSG_PROT_MODE ; Use our 32 - bit print routine to
    call print_string_pm ; announce we are in protected mode
    call KERNEL_OFFSET ; Now jump to the address of our loaded
    ; kernel code , assume the brace position ,
    ; and cross your fingers. Here we go!
    jmp $ ; Hang.


; Global variables
BOOT_DRIVE db 0
MSG_REAL_MODE db "Started in 16 - bit Real Mode" , 0
MSG_PROT_MODE db "Successfully landed in 32 - bit Protected Mode" , 0
MSG_LOAD_KERNEL db "Loading kernel into memory." , 0

; Bootsector padding
times 510 -( $ - $$ ) db 0
dw 0xaa55