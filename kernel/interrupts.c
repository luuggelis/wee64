#include <stdint.h>
#include "io.h"

extern void print(const char *str);
extern void keyboard_interrupt(void);

#define INPUT_SIZE 128

static char input_buff[INPUT_SIZE];
static int input_length;
static int shift_pressed = 0;

volatile int line_ready = 0;
char line_buff[INPUT_SIZE];

struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  ist;
    uint8_t  type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t zero;
} __attribute__((packed));

struct idt_ptr
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idt_descriptor;

static void pic_remap(void)
{
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    outb(0x21, 0x20);
    outb(0xA1, 0x28);

    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    outb(0x21, 0xFD);
    outb(0xA1, 0xFF);
}

static void idt_set_gate(int number, void (*handler)(void))
{
    uint64_t address = (uint64_t)handler;

    idt[number].offset_low  = address & 0xFFFF;
    idt[number].selector    = 0x08;
    idt[number].ist         = 0;
    idt[number].type_attr   = 0x8E;
    idt[number].offset_mid  = (address >> 16) & 0xFFFF;
    idt[number].offset_high = (address >> 32) & 0xFFFFFFFF;
    idt[number].zero        = 0;
}

void idt_init(void)
{
    idt_descriptor.limit = sizeof(idt) - 1;
    idt_descriptor.base = (uint64_t)idt;

    print("1\n");

    for (int i = 0; i < 256; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].ist = 0;
        idt[i].type_attr = 0;
        idt[i].offset_mid = 0;
        idt[i].offset_high = 0;
        idt[i].zero = 0;
    }

    print("2\n");

    idt_set_gate(33, keyboard_interrupt);

    print("3\n");

    pic_remap();

    print("4\n");

    __asm__ volatile ("lidt %0" : : "m"(idt_descriptor));

    print("5\n");
}

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36)
    {
        shift_pressed = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6)
    {
        shift_pressed = 0;
        return;
    }

    if (scancode & 0x80)
    {
        return;
    }

    if (scancode == 0x1C)
    {
        input_buff[input_length] = '\0';
        print("\n");

        for (int i = 0; i <= input_length; i++)
        {
            line_buff[i] = input_buff[i];
        }

        line_ready = 1;
        input_length = 0;

        return;
    }

    char c = 0;

    if (scancode == 0x0E)
    {
        if (input_length > 0)
        {
            input_length--;

            print("\b");
        }

        return;
    }

    switch (scancode)
    {
        case 0x1E: c = 'a'; break;
        case 0x30: c = 'b'; break;
        case 0x2E: c = 'c'; break;
        case 0x20: c = 'd'; break;
        case 0x12: c = 'e'; break;
        case 0x21: c = 'f'; break;
        case 0x22: c = 'g'; break;
        case 0x23: c = 'h'; break;
        case 0x17: c = 'i'; break;
        case 0x24: c = 'j'; break;
        case 0x25: c = 'k'; break;
        case 0x26: c = 'l'; break;
        case 0x32: c = 'm'; break;
        case 0x31: c = 'n'; break;
        case 0x18: c = 'o'; break;
        case 0x19: c = 'p'; break;
        case 0x10: c = 'q'; break;
        case 0x13: c = 'r'; break;
        case 0x1F: c = 's'; break;
        case 0x14: c = 't'; break;
        case 0x16: c = 'u'; break;
        case 0x2F: c = 'v'; break;
        case 0x11: c = 'w'; break;
        case 0x2D: c = 'x'; break;
        case 0x15: c = 'y'; break;
        case 0x2C: c = 'z'; break;
        case 0x39: c = ' '; break;
        case 0x02: c = '1'; break;
        case 0x03: c = '2'; break;
        case 0x04: c = '3'; break;
        case 0x05: c = '4'; break;
        case 0x06: c = '5'; break;
        case 0x07: c = '6'; break;
        case 0x08: c = '7'; break;
        case 0x09: c = '8'; break;
        case 0x0A: c = '9'; break;
        case 0x0B: c = '0'; break;
    }

    if (shift_pressed && c >= 'a' && c <= 'z')
    {
        c -= 'a' - 'A';
    }

    if (c != 0 && input_length < INPUT_SIZE - 1)
    {
        input_buff[input_length] = c;
        input_length++;

        char str[2];

        str[0] = c;
        str[1] = '\0';

        print(str);
    }
}
