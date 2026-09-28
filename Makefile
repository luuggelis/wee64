AS = nasm
CC = gcc
LD = ld

VERSION = 0.1.0
ISO     = wee64-$(VERSION).iso
BUILD   = build

CFLAGS  = -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone \
          -mgeneral-regs-only -Wall -Wextra -MMD -MP
LDFLAGS = -m elf_x86_64 -T linker.ld

C_SRCS   = kernel/kernel.c kernel/interrupts.c kernel/shell.c kernel/string.c
C_OBJS   = $(patsubst kernel/%.c,$(BUILD)/%.o,$(C_SRCS))
ASM_OBJS = $(BUILD)/boot.o $(BUILD)/interrupts_asm.o
OBJS     = $(ASM_OBJS) $(C_OBJS)

.PHONY: all clean run

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot/boot.asm | $(BUILD)
	$(AS) -f elf64 $< -o $@

$(BUILD)/interrupts_asm.o: kernel/interrupts.asm | $(BUILD)
	$(AS) -f elf64 $< -o $@

$(BUILD)/%.o: kernel/%.c | $(BUILD)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(ISO): $(BUILD)/kernel.bin boot/grub.cfg
	mkdir -p iso/boot/grub
	cp $(BUILD)/kernel.bin iso/boot/kernel.bin
	cp boot/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso

run: $(ISO)
	qemu-system-x86_64 -cdrom $(ISO)

clean:
	rm -rf $(BUILD) iso $(ISO)

-include $(C_OBJS:.o=.d)
