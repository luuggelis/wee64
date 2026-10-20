AS = nasm
CC = gcc
LD = ld
OBJCOPY = objcopy

VERSION = 0.3.1
ISO     = wee64-$(VERSION).iso
BUILD   = build

DISK := disks/wdata.img

CFLAGS  = -m64 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone \
          -mgeneral-regs-only -Wall -Wextra -MMD -MP \
          -DWEE64_VERSION=\"$(VERSION)\"

LDFLAGS = -m elf_x86_64 -T linker.ld

C_SRCS   = kernel/kernel.c kernel/interrupts.c kernel/shell.c kernel/string.c kernel/diagnostics.c kernel/exceptions.c \
		   kernel/ata.c fs/fat32.c

ART_TXT = assets/art.txt
ART_OBJ = $(BUILD)/art.o

C_OBJS = $(patsubst %.c,$(BUILD)/%.o,$(C_SRCS))
ASM_OBJS = $(BUILD)/boot.o $(BUILD)/interrupts_asm.o $(BUILD)/exceptions_asm.o
OBJS     = $(ASM_OBJS) $(C_OBJS) $(ART_OBJ)

.PHONY: all clean run

all: $(ISO)

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/boot.o: boot/boot.asm | $(BUILD)
	$(AS) -f elf64 $< -o $@

$(BUILD)/interrupts_asm.o: kernel/interrupts.asm | $(BUILD)
	$(AS) -f elf64 $< -o $@

$(BUILD)/exceptions_asm.o: kernel/exceptions.asm | $(BUILD)
	$(AS) -f elf64 $< -o $@

$(ART_OBJ): $(ART_TXT) | $(BUILD)
	$(OBJCOPY) -I binary -O elf64-x86-64 -B i386:x86-64 $< $@

$(BUILD)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/kernel.bin: $(OBJS) linker.ld
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(ISO): $(BUILD)/kernel.bin boot/grub.cfg
	mkdir -p iso/boot/grub
	cp $(BUILD)/kernel.bin iso/boot/kernel.bin
	cp boot/grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -o $@ iso

run: $(ISO)
	qemu-system-x86_64 \
		-cdrom $(ISO) \
		-boot order=d \
		-display gtk \
		-drive file=$(DISK),format=raw,if=ide,index=0

clean:
	rm -rf $(BUILD) iso $(ISO)

-include $(C_OBJS:.o=.d)
