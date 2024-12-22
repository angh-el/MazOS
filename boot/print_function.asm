print_function:
    pusha                 ; Save all general-purpose registers
    mov ah, 0x0e          ; Set BIOS teletype output function
print_function_loop:
    mov al, [bx]          ; Load the current character from the string
    cmp al, 0             ; Check for null terminator
    je print_function_exit ; Exit the function if the null terminator is found
    int 0x10              ; Print the character
    inc bx                ; Move to the next character
    jmp print_function_loop ; Repeat for the next character
print_function_exit:
    popa                  ; Restore all general-purpose registers
    ret                   ; Return to the caller
