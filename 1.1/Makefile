# might have to use -ffreestanding
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
	gcc -m32 -fno-stack-protector -fno-builtin -c libs/process.c -o process.o
	gcc -m32 -fno-stack-protector -fno-builtin -c timer.c -o timer.o
	gcc -m32 -fno-stack-protector -fno-builtin -c programs/calculator.c -o calculator.o
	gcc -m32 -fno-stack-protector -fno-builtin -c programs/snake.c -o snake.o
	gcc -m32 -fno-stack-protector -fno-builtin -c drivers/disk_io.c -o disk_io.o
	gcc -m32 -fno-stack-protector -fno-builtin -c drivers/fat32.c -o fat32.o

boot:
	nasm -f elf32 boot.s -o boot.o
	nasm -f elf32 gdt_flush.s -o gdt_flush.o
	nasm -f elf32 idt_flush.s -o idt_flush.o
	nasm -f elf32 libs/process_s.s -o process_s.o


image:
	ld -m elf_i386 -T linker.ld -o kernel boot.o kernel.o screen.o gdt.o gdt_flush.o util.o idt.o idt_flush.o printf.o keyboard.o timer.o memory_init.o kernel_memory.o process.o process_s.o calculator.o snake.o disk_io.o fat32.o
	mv kernel os/boot/kernel

	mkdir -p os/files
#	cp testfile.txt os/files/

	
#	cp filesystem.img os/boot/
	cp filesystem.img os/files/

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






####################################################################################
# # # how to make the fat 32 and mount it to the os and shii

# # Create a 100MB FAT32 filesystem image
# dd if=/dev/zero of=filesystem.img bs=1M count=50
# mkfs.fat -F 32 filesystem.img


# # Mount the filesystem image to a directory
# mkdir /mnt/fat32
# sudo mount -o loop filesystem.img /mnt/fat32

# # Copy your files from your 'os/files/' directory into the mounted image
# sudo cp -r os/files/* /mnt/fat32/

# # Unmount the image once done
# sudo umount /mnt/fat32
