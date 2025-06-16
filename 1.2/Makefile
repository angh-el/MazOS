all: clean kernel boot image

clean:
	rm -rf *.o

kernel:
	g++ -m32 -fno-stack-protector -fno-builtin -c kernel.cpp -o kernel.o


boot:
	nasm -f elf32 boot.s -o boot.o



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
	