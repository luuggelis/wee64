bits 32

section .multiboot
align 8

header_start:
    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    ; Multiboot2 end tag
    dw 0
    dw 0
    dd 8

header_end:

section .bss
align 4096

page_table:
    resb 4096

section .text
global _start
extern kernel_main

_start:

.hang:
    cli
    hlt
    jmp .hang
