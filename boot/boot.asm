section .multiboot
align 8

    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    ; End tag
    dw 0
    dw 0
    dd 8

header_start:

header_end:

section .text
bits 32

global _start
extern kernel_main

_start:
    call kernel_main

.hang:
    cli
    hlt
    jmp .hang
