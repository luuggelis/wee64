<img width="800" height="350" alt="koivulogo" src="https://github.com/user-attachments/assets/97c8396a-f054-495a-a290-121c21f699fc" />

# koivu

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
git clone https://github.com/luuggelis/koivu.git
cd wee64
make
make run
```

## structure

- `boot/` — bootloader and GRUB config
- `kernel/` — kernel source (C + assembly)
- `linker.ld` — linker script
