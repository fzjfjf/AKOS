# Varijable za kompajlere i zastavice
ASM = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -c -fno-pic -Iclangh
ASMFLAGS_BIN = -f bin
ASMFLAGS_ELF = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld --oformat binary

# Glavno pravilo
all: disk.img

# asemblaj in-out-funkcije
obj/in-out-functions.o: asm/in-out-functions.asm
	@echo "Assembling in and out functions"
	$(ASM) $(ASMFLAGS_ELF) asm/in-out-functions.asm -o obj/in-out-functions.o
# Spajanje bootloadera i kernela u finalnu sliku diska
disk.img: bin/bootloader.bin bin/init_kernel.bin
	@echo "Generating image..."
	cat bin/bootloader.bin bin/init_kernel.bin > disk.img

# Kompajliranje bootloadera
bin/bootloader.bin: asm/bootloader.asm
	@echo "Building bootloader..."
	$(ASM) $(ASMFLAGS_BIN) asm/bootloader.asm -o bin/bootloader.bin

# Linkovanje kernela (Uključuje initializer na početku i signature na samom kraju!)
bin/init_kernel.bin: obj/initializer.o obj/kernel.o obj/kernel_signature.o obj/kstdlib.o obj/in-out-functions.o
	@echo "Linking..."
	$(LD) $(LDFLAGS) obj/initializer.o obj/kernel.o obj/kstdlib.o obj/in-out-functions.o obj/kernel_signature.o -o bin/init_kernel.bin

# Asemblanje potpisa u objektni fajl (.o) u ELF32 formatu
obj/kernel_signature.o: asm/kernel_signature.asm
	@echo "Assembling signature..."
	$(ASM) $(ASMFLAGS_ELF) asm/kernel_signature.asm -o obj/kernel_signature.o

# Kompajliranje asm inicijalizatora
obj/initializer.o: asm/initializer.asm
	$(ASM) $(ASMFLAGS_ELF) asm/initializer.asm -o obj/initializer.o

# Kompajliranje C kernela (prati i izmjene u kstdlib.h)
obj/kernel.o: clang/kernel.c clangh/kstdlib.h
	@echo "Building kernel..."
	$(CC) $(CFLAGS) clang/kernel.c -o obj/kernel.o

# kompajluj kstdlib
obj/kstdlib.o: clang/kstdlib.c clangh/kstdlib.h
	@echo "Building standard library..."
	$(CC) $(CFLAGS) clang/kstdlib.c -o obj/kstdlib.o

# Pravilo za pokretanje u QEMU
run: disk.img
	@echo "Running the image..."
	dd if=/dev/null of=disk.img bs=512 seek=61
	qemu-system-i386 -m 4096 disk.img

# Čišćenje generisanih fajlova
clean:
	@echo "Cleaning up..."
	rm -f bin/*.bin obj/*.o disk.img
