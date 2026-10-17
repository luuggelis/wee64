<img width="345" height="183" alt="juu" src="https://github.com/user-attachments/assets/b6e40dfc-3bfa-4274-9e33-d3e9b2d04016" />

# wee64

### a small, lightweight operating system/kernel written in C/Assembly

## status

VERY early in development, don't use (yet).

on version 0.2.3

## requirements

- some sort of ISO emulator (e.g. qemu)
- gcc (C compiler)
- nasm (assembly compiler)
- grub
- ld (or binutils)
- xorriso (to make it an ISO image)
- mtools
- make

ubuntu/debian-based linux distro is recommended, but wsl on windows works fine too

## building & running

```bash
git clone https://github.com/luuggelis/wee64.git
cd wee64
make
make run
```

## structure

- `boot/` — bootloader and GRUB config
- `kernel/` — kernel source (C + assembly)
- `linker.ld` — linker script
