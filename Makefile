AS = nasm
CC = gcc
LD = ld

CFLAGS = -m32 -ffreestanding -fno-pie -fno-stack-protector
LDFLAGS = -m elf_i386 -T linker.ld

all: wee64.iso

boot.o: boot/boot.asm
	$(AS) -f elf32 $< -o $@

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

kernel.bin: boot.o kernel.o linker.ld
	$(LD) $(LDFLAGS) boot.o kernel.o -o $@

wee64.iso: kernel.bin
	mkdir -p iso/boot
	cp kernel.bin iso/boot/kernel.bin
	grub-mkrescue -o wee64.iso iso

clean:
	rm -rf *.o *.bin wee64.iso iso
