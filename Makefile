CF = -m32 -c -ffreestanding --freestanding -fno-pic -Isrc/clangh -Isrc/clangh/stdlib -Isrc/clangh/keyboard -Isrc/clangh/shell -Isrc/clangh/pong
O = compiled/obj

OBJS = $(O)/initializer.o $(O)/kernel.o $(O)/kstdlib.o $(O)/keyboard.o $(O)/inout.o $(O)/shell.o $(O)/idt.o $(O)/pong.o $(O)/idtc.o $(O)/signature.o

all: disk.img

compiled/obj/idtc.o: src/clang/idt.c
	@echo "Building IDT (C part)..."
	@gcc $(CF) src/clang/idt.c -o compiled/obj/idtc.o

compiled/obj/idt.o: src/asm/idt.asm
	@echo "Building IDT..."
	@nasm -f elf32 src/asm/idt.asm -o compiled/obj/idt.o

compiled/bin/bootloader.bin: src/asm/bootloader.asm
	@echo "Building bootloader..."
	@nasm -f bin src/asm/bootloader.asm -o compiled/bin/bootloader.bin

$(O)/initializer.o: src/asm/initializer.asm
	@echo "Building initializer..."
	@nasm -f elf32 src/asm/initializer.asm -o compiled/obj/initializer.o

$(O)/pong.o: src/clang/pong/pong.c
	@echo "Building pong..."
	@gcc $(CF) src/clang/pong/pong.c -o $(O)/pong.o

$(O)/kstdlib.o: src/clang/stdlib/kstdlib.c src/clangh/stdlib/kstdlib.h src/clangh/keyboard/keyboard.h
	@echo "Building standard library..."
	@gcc $(CF) src/clang/stdlib/kstdlib.c -o compiled/obj/kstdlib.o

$(O)/keyboard.o: src/clang/keyboard/keyboard.c src/clangh/keyboard/keyboard.h
	@echo "Building keyboard driver..."
	@gcc $(CF) src/clang/keyboard/keyboard.c -o compiled/obj/keyboard.o

$(O)/kernel.o: src/clang/kernel.c src/clangh/stdlib/kstdlib.h src/clangh/pong/pong.h
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

clean:
	@echo "Cleaning compiled/"
	@rm -rf compiled/bin/*
	@rm -rf compiled/obj/*