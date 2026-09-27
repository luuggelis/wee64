AS = nasm
CC = gcc
LD = ld

CFLAGS = -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone
LDFLAGS = -m elf_x86_64 -T linker.ld

all: wee64.iso

boot.o: boot/boot.asm
	$(AS) -f elf64 $< -o $@

kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

interrupts.o: kernel/interrupts.c kernel/io.h
	$(CC) $(CFLAGS) -c $< -o $@

interrupts_asm.o: kernel/interrupts.asm
	$(AS) -f elf64 $< -o $@

kernel.bin: boot.o kernel.o interrupts.o interrupts_asm.o linker.ld
	$(LD) $(LDFLAGS) boot.o kernel.o interrupts.o interrupts_asm.o -o $@

wee64.iso: kernel.bin
	mkdir -p iso/boot/grub
	cp kernel.bin iso/boot/kernel.bin
	cp boot/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o wee64.iso iso

clean:
	rm -rf *.o *.bin wee64.iso iso
