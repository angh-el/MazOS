
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
	gcc -m32 -fno-stack-protector -fno-builtin -c libs/kernel_memory.c -o kernel_memory.o

	gcc -m32 -fno-stack-protector -fno-builtin -c timer.c -o timer.o

boot:
	nasm -f elf32 boot.s -o boot.o
	nasm -f elf32 gdt_flush.s -o gdt_flush.o
	nasm -f elf32 idt_flush.s -o idt_flush.o


image:
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o screen.o gdt.o gdt_flush.o util.o idt.o idt_flush.o printf.o keyboard.o timer.o memory_init.o kernel_memory.o
	mv kernel os/boot/kernel

	mkdir -p os/files
	cp testfile.txt os/files/

	grub-mkrescue -o os.iso os/
	rm *.o

run: os.iso
	qemu-system-i386 os.iso






# all: clean kernel boot image

# clean:
# 	rm -rf *.o

# kernel:
# 	gcc -ffreestanding -g -c kernel.c -o kernel.o
# 	gcc -ffreestanding -g -c drivers/screen.c -o screen.o
# 	gcc -ffreestanding -g -c gdt.c -o gdt.o
# 	gcc -ffreestanding -g -c libs/util.c -o util.o
# 	gcc -ffreestanding -g -c idt.c -o idt.o
# 	gcc -ffreestanding -g -c libs/printf.c -o printf.o
# 	gcc -ffreestanding -g -c drivers/keyboard.c -o keyboard.o
# 	gcc -ffreestanding -g -c libs/memory_init.c -o memory_init.o
# 	gcc -ffreestanding -g -c libs/kernel_memory.c -o kernel_memory.o
# 	gcc -ffreestanding -g -c timer.c -o timer.o

# boot:
# 	nasm -f elf32 boot.s -o boot.o
# 	nasm -f elf32 gdt_flush.s -o gdt_flush.o
# 	nasm -f elf32 idt_flush.s -o idt_flush.o


# image:
# 	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o screen.o gdt.o gdt_flush.o util.o idt.o idt_flush.o printf.o keyboard.o timer.o memory_init.o kernel_memory.o
# 	mv kernel os/boot/kernel

# 	mkdir -p os/files
# 	cp testfile.txt os/files/

# 	grub-mkrescue -o os.iso os/
# 	rm *.o

# run: os.iso
# 	qemu-system-i386 os.iso
