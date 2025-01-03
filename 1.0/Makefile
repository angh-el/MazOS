# Automatically generate lists of sources using wildcards .
C_SOURCES = $(wildcard kernel/*.c drivers/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h)

# TODO : Make sources dep on all header files .

# Convert the *.c filenames to *.o to give a list of object files to build
OBJ = ${C_SOURCES:.c=.o}

# Defaul build target
all : os-image

# Run qemu to simulate booting of our code
run : all
	qemu-system-x86_64 os-image

# This is the actual disk image that the computer loads
# which is the combination of our compiled bootsector and kernel
os-image : boot/boot.bin kernel.bin
	cat $^ > os-image

# This builds the binary of kernel from two object files :
kernel.bin : kernel/kernel.o ${OBJ}
	ld -o $@ -Ttext 0x1000 $^ --oformat binary

# Generic rule for compiling C code to an object file
# For simplicity , we C files depend on all header files .
%.o : %.c ${HEADERS}
	gcc -ffreestanding -c $< -o $@
	# gcc -ffreestanding -nostdlib -c $< -o $@

# Assemble the kernel
%.o : %.asm
	nasm $< -f elf -o $@

%.bin : %.asm
	nasm $< -f bin -I '../../16bit/' -o $@
	# nasm $< -f bin -o $@

clean :
	rm -fr *.bin *.dis *.o os-image
	rm -fr kernel/*.o boot/*.bin drivers/*.o
