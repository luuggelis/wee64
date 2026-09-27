<img width="418" height="149" alt="weelogo" src="https://github.com/user-attachments/assets/03ce344e-f4ec-4938-87e5-1cfd7f1d4d10" />

# wee64

### a small, lightweight operating system/kernel written in C/Assembly

## status

VERY early in development, don't use (yet).

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
qemu-system-x86_64 -cdrom wee64.iso
```

## structure

- `boot/` — bootloader and GRUB config
- `kernel/` — kernel source (C + assembly)
- `linker.ld` — linker script

---

*wee bit small, ain't it?*
