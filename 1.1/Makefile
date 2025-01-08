# all: os.iso
# 	gcc -m32 -fno-stack-protector -fno-builtin -c kernel.c -o kernel.o
# 	nasm -f elf32 boot.s -o boot.o
# 	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o
# 	mv kernel os/boot/kernel
# 	grub-mkrescue -o os.iso os/

# run: os.iso
# 	qemu-system-i386 os.iso




######################################################################################


# # Compiler and flags
# CC = gcc
# CFLAGS = -m32 -fno-stack-protector -fno-builtin
# AS = nasm
# LD = ld

# # Source and object files
# C_SOURCES = $(wildcard *.c)
# C_OBJECTS = $(C_SOURCES:.c=.o)

# # Assembly and linker scripts
# ASM_SOURCES = boot.s
# ASM_OBJECTS = $(ASM_SOURCES:.s=.o)

# LINKER_SCRIPT = linker.ld
# KERNEL_BIN = kernel

# # Output files
# ISO_DIR = os/boot
# ISO_FILE = os.iso

# all: $(ISO_FILE)

# # Compile C files
# %.o: %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# # Compile assembly files
# %.o: %.s
# 	$(AS) -f elf32 $< -o $@

# # Link kernel binary
# $(KERNEL_BIN): $(ASM_OBJECTS) $(C_OBJECTS)
# 	$(LD) -m elf_i386 -T $(LINKER_SCRIPT) -o $@ $(ASM_OBJECTS) $(C_OBJECTS)

# # Move kernel to ISO directory
# $(ISO_FILE): $(KERNEL_BIN)
# 	mkdir -p $(ISO_DIR)
# 	mv $(KERNEL_BIN) $(ISO_DIR)/kernel
# 	grub-mkrescue -o $@ os/

# # Run the kernel in QEMU
# run: $(ISO_FILE)
# 	qemu-system-i386 $<

# # Clean up build files
# clean:
# 	rm -f $(C_OBJECTS) $(ASM_OBJECTS) $(KERNEL_BIN) $(ISO_FILE)
# 	rm -rf $(ISO_DIR)/kernel

# .PHONY: all run clean


##########################################################

all: clean kernel boot image

clean:
	rm -rf *.o

kernel:
	gcc -m32 -fno-stack-protector -fno-builtin -c kernel.c -o kernel.o
	gcc -m32 -fno-stack-protector -fno-builtin -c drivers/screen.c -o screen.o
	gcc -m32 -fno-stack-protector -fno-builtin -c gdt.c -o gdt.o
	gcc -m32 -fno-stack-protector -fno-builtin -c libs/util.c -o util.o
	gcc -m32 -fno-stack-protector -fno-builtin -c idt.c -o idt.o
	gcc -m32 -fno-stack-protector -fno-builtin -c libs/printf.c -o printf.o
	gcc -m32 -fno-stack-protector -fno-builtin -c drivers/keyboard.c -o keyboard.o
	gcc -m32 -fno-stack-protector -fno-builtin -c libs/memory_init.c -o memory_init.o

	gcc -m32 -fno-stack-protector -fno-builtin -c timer.c -o timer.o

boot:
	nasm -f elf32 boot.s -o boot.o
	nasm -f elf32 gdt_flush.s -o gdt_flush.o
	nasm -f elf32 idt_flush.s -o idt_flush.o

image:
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o screen.o gdt.o gdt_flush.o util.o idt.o idt_flush.o printf.o keyboard.o timer.o memory_init.o
	mv kernel os/boot/kernel
	grub-mkrescue -o os.iso os/
	rm *.o

run: os.iso
	qemu-system-i386 os.iso
