#include <stdint.h>
#include "io.h"

extern void print(const char *str);
extern void keyboard_interrupt(void);

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

    __asm__ volatile ("lidt %0" : : "m"(idt_descriptor));
}

void keyboard_handler(void)
{
    uint8_t scancode = inb(0x60);

    if (scancode == 0x1E)
    {
        print("a");
    }
}
