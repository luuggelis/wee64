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

    idt_set_gate(33, keyboard_interrupt);

    pic_remap();

    __asm__ volatile ("lidt %0" : : "m"(idt_descriptor));
}

static const char sc_normal[] =
{
    /* 0x00 */ 0,    0,   '1', '2', '3', '4', '5', '6',
    /* 0x08 */ '7',  '8', '9', '0', '-', '=', 0,   0,
    /* 0x10 */ 'q',  'w', 'e', 'r', 't', 'y', 'u', 'i',
    /* 0x18 */ 'o',  'p', '[', ']', 0,   0,   'a', 's',
    /* 0x20 */ 'd',  'f', 'g', 'h', 'j', 'k', 'l', ';',
    /* 0x28 */ '\'', '`', 0,   '\\','z', 'x', 'c', 'v',
    /* 0x30 */ 'b',  'n', 'm', ',', '.', '/', 0,   '*',
    /* 0x38 */ 0,    ' '
};

static const char sc_shift[] =
{
    /* 0x00 */ 0,    0,   '!', '@', '#', '$', '%', '^',
    /* 0x08 */ '&',  '*', '(', ')', '_', '+', 0,   0,
    /* 0x10 */ 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    /* 0x18 */ 'O',  'P', '{', '}', 0,   0,   'A', 'S',
    /* 0x20 */ 'D',  'F', 'G', 'H', 'J', 'K', 'L', ':',
    /* 0x28 */ '"',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    /* 0x30 */ 'B',  'N', 'M', '<', '>', '?', 0,   '*',
    /* 0x38 */ 0,    ' '
};

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

    if (scancode < sizeof(sc_normal))
    {
        c = shift_pressed ? sc_shift[scancode] : sc_normal[scancode];
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
