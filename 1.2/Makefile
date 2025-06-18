all: clean kernel boot image

clean:
	rm -rf *.o

kernel:
# 	kernel.cpp
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c kernel.cpp -o kernel.o

#  	uitl.cpp
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c lib/util.cpp -o util.o

#	descriptors (idt.cpp & gdt.cpp)	  
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c descriptors/gdt.cpp -o gdt.o
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c descriptors/idt.cpp -o idt.o
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c descriptors/idt_wrapper.cpp -o idt_wrapper.o	

# 	managers (memory, heap and process)
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c managers/memory_manager.cpp -o memory_manager.o
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c managers/heap_manager.cpp -o heap_manager.o
	g++ -m32 -ffreestanding -fno-stack-protector -fno-builtin -c managers/process_manager.cpp -o process_manager.o

boot:
	nasm -f elf32 boot.s -o boot.o
	nasm -f elf32 descriptors/gdt_flush.s -o gdt_flush.o
	nasm -f elf32 descriptors/idt_flush.s -o idt_flush.o
	nasm -f elf32 managers/process_manager_s.s -o process_manager_s.o

image:
#	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o 
	ld -m elf_i386 -T linker.ld -o kernel *.o
	mv kernel MazOS/boot/kernel

	mkdir -p MazOS/files
#	cp testfile.txt os/files/

	
#	cp filesystem.img os/boot/
	cp filesystem.img MazOS/files/

	grub-mkrescue -o MazOS.iso MazOS/
	rm *.o

run: MazOS.iso
	qemu-system-i386 MazOS.iso
	