bits 32

section .multiboot
align 8

header_start:
    dd 0xE85250D6
    dd 0
    dd header_end - header_start
    dd -(0xE85250D6 + 0 + (header_end - header_start))

    dw 0
    dw 0
    dd 8

header_end:


section .bss
align 4096

pml4:
    resb 4096

pdpt:
    resb 4096

pd:
    resb 4096

align 16
stack_bottom:
    resb 16384
stack_top:


section .text
global _start
extern kernel_main

_start:

    mov eax, pdpt
    or eax, 0x3
    mov [pml4], eax

    mov eax, pd
    or eax, 0x3
    mov [pdpt], eax

    mov eax, 0x83
    mov [pd], eax

    mov eax, pml4
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [gdt64_descriptor]

    ; jump into 64-bit code
    jmp 0x08:long_mode_start


bits 64

long_mode_start:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    mov rsp, stack_top

    call kernel_main

.hang:
    cli
    hlt
    jmp .hang

align 8

gdt64:
    dq 0x0000000000000000
    dq 0x00AF9A000000FFFF
    dq 0x00CF92000000FFFF

gdt64_end:

gdt64_descriptor:
    dw gdt64_end - gdt64 - 1
    dq gdt64
