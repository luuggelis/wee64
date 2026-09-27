bits 64

section .text
global keyboard_interrupt
extern keyboard_handler

keyboard_interrupt:
    push rax

    call keyboard_handler

    mov al, 0x20
    out 0x20, al

    pop rax

    iretq
