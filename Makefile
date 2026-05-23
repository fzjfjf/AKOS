CF = -m32 -c -ffreestanding --freestanding -fno-pic -Isrc/clangh -Isrc/clangh/stdlib -Isrc/clangh/keyboard -Isrc/clangh/shell
O = compiled/obj

OBJS = $(O)/initializer.o $(O)/kernel.o $(O)/kstdlib.o $(O)/keyboard.o $(O)/inout.o $(O)/shell.o $(O)/signature.o

all: disk.img

compiled/bin/bootloader.bin: src/asm/bootloader.asm
	@echo "Building bootloader..."
	@nasm -f bin src/asm/bootloader.asm -o compiled/bin/bootloader.bin

$(O)/initializer.o: src/asm/initializer.asm
	@echo "Building initializer..."
	@nasm -f elf32 src/asm/initializer.asm -o compiled/obj/initializer.o

$(O)/kstdlib.o: src/clang/stdlib/kstdlib.c src/clangh/stdlib/kstdlib.h src/clangh/keyboard/keyboard.h
	@echo "Building standard library..."
	@gcc $(CF) src/clang/stdlib/kstdlib.c -o compiled/obj/kstdlib.o

$(O)/keyboard.o: src/clang/keyboard/keyboard.c src/clangh/keyboard/keyboard.h
	@echo "Building keyboard driver..."
	@gcc $(CF) src/clang/keyboard/keyboard.c -o compiled/obj/keyboard.o

$(O)/kernel.o: src/clang/kernel.c src/clangh/stdlib/kstdlib.h
	@echo "Building kernel..."
	@gcc $(CF) src/clang/kernel.c -o compiled/obj/kernel.o

$(O)/signature.o: src/asm/kernel_signature.asm 
	@echo "Building signature..."
	@nasm -f elf32 src/asm/kernel_signature.asm -o compiled/obj/signature.o

$(O)/inout.o: src/asm/in-out-functions.asm 
	@echo "Building inb and outb functions..."
	@nasm -f elf32 src/asm/in-out-functions.asm -o compiled/obj/inout.o

$(O)/shell.o: src/clang/shell/shell.c src/clangh/keyboard/keyboard.h src/clangh/stdlib/kstdlib.h
	@echo "Building shell..."
	@gcc $(CF) src/clang/shell/shell.c -o $(O)/shell.o

compiled/bin/combined.bin: $(OBJS)
	@echo "Linking everything..."
	@ld -m elf_i386 -T linker.ld --oformat binary $(OBJS) -o compiled/bin/combined.bin
 
disk.img: compiled/bin/bootloader.bin compiled/bin/combined.bin
	@echo "Making disk image..."
	@cat compiled/bin/bootloader.bin compiled/bin/combined.bin > disk.img
	@dd if=/dev/null of=disk.img bs=512 seek=61

run: disk.img
	@echo "Running image in QEMU..."
	@qemu-system-i386 -m 4096 -drive format=raw,file=disk.img	
