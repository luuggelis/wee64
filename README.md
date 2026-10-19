<img width="153" height="100" alt="ascii-art-text(1)" src="https://github.com/user-attachments/assets/6c03012d-c627-4694-8e6d-3f946bbb6b56" />

# wee64

### a small, lightweight operating system/kernel written in C/Assembly

## status

VERY early in development, don't use (yet).

on version 0.3.0

## requirements

- either VirtualBox or QEMU
- gcc (C compiler)
- nasm (assembly compiler)
- grub
- ld (or binutils)
- xorriso (to make it an ISO image)
- mtools
- make

a Linux system is recommended, but wsl on windows works fine too.

## building & running

```bash
git clone https://github.com/luuggelis/wee64.git
cd wee64
./run.sh
```

## structure

- `boot/` — bootloader and GRUB config
- `kernel/` — kernel source (C + assembly)
- `fs/` — filesystem code (mostly C)
- `linker.ld` — linker script
