#!/bin/bash

# build the bootloader
echo "Building bootloader..."
nasm -f bin asm/bootloader.asm -o bin/bootloader.bin

# generate object files
echo "Building kernel..."
nasm -f elf32 asm/initializer.asm -o obj/initializer.o
gcc -m32 --freestanding -c -fno-pic clang/kernel.c -o obj/kernel.o

# link
echo "Linking..."
ld -m elf_i386 -T linker.ld obj/initializer.o obj/kernel.o -o bin/init_kernel.bin --oformat binary

# stitch together
echo "Generating image..."
cat bin/bootloader.bin bin/init_kernel.bin > disk.img

# run
echo Running the image
qemu-system-i386 disk.img
