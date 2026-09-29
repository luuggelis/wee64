#include <stdint.h>

extern void print(const char *str);

extern char _kernel_start;
extern char _kernel_end;

static void print_uint(uint64_t value)
{
    char buffer[21];
    int i = 0;

    if (value == 0)
    {
        print("0");
        return;
    }

    while (value > 0)
    {
        buffer[i++] = '0' + (value % 10);
        value /= 10;
    }

    while (i > 0)
    {
        char str[2];

        str[0] = buffer[--i];
        str[1] = '\0';

        print(str);
    }
}

static void print_hex(uint64_t value)
{
    const char *hex = "0123456789ABCDEF";

    print("0x");

    for (int i = 15; i >= 0; i--)
    {
        char str[2];

        str[0] = hex[(value >> (i * 4)) & 0xF];
        str[1] = '\0';

        print(str);
    }
}

void diagnostics(void)
{
    uint64_t kernel_start = (uint64_t)&_kernel_start;
    uint64_t kernel_end = (uint64_t)&_kernel_end;
    uint64_t kernel_size = kernel_end - kernel_start;

    print("wee64 diagnostics\n");
    print("-----------------\n");

    print("\nkernel\n");
    print("  start: ");
    print_hex(kernel_start);
    print("\n");

    print("  end:   ");
    print_hex(kernel_end);
    print("\n");

    print("  size:  ");
    print_uint(kernel_size);
    print(" bytes\n");
}
