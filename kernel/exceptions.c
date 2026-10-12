#include <stdint.h>

extern void print(const char *str);

static const char *const exception_names[32] = {
    "Divide Error",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

static void print_uint(uint64_t value)
{
    char buffer[21];
    unsigned i = 0;

    if (value == 0)
    {
        print("0");

        return;
    }

    while (value != 0) {
        buffer[i++] = '0' + value % 10;
        value /= 10;
    }

    while (i != 0) {
        char text[2] = { buffer[--i], '\0' };
        print(text);
    }
}

__attribute__((noreturn))
void exception_handler(uint64_t vector, uint64_t error_code)
{
    print("\n\n*** KOIVU KERNEL EXCEPTION ***\n");

    if (vector < 32) {
        print("exception: ");
        print(exception_names[vector]);
        print("\n");
    }

    print("vector: ");
    print_uint(vector);
    print("\nerror code: ");
    print_uint(error_code);
    print("\n");

    if (vector == 14) {
        uint64_t address;

        __asm__ volatile (
            "mov %%cr2, %0"
            : "=r"(address)
        );

        print("page fault address: 0x");

        // Print CR2 in hexadecimal.
        const char *hex = "0123456789ABCDEF";

        for (int i = 15; i >= 0; i--) {
            char text[2] = {
                hex[(address >> (i * 4)) & 0xF],
                '\0'
            };
            print(text);
        }

        print("\n");
    }

    print("system halted\n");

    for (;;) {
        __asm__ volatile ("cli; hlt" ::: "memory");
    }
}
