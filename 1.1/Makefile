all: os.iso
	gcc -m32 -fno-stack-protector -fno-builtin -c kernel.c -o kernel.o
	nasm -f elf32 boot.s -o boot.o
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o
	mv kernel os/boot/kernel
	grub-mkrescue -o os.iso os/

run: os.iso
	qemu-system-i386 os.iso