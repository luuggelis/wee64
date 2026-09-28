#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern void idt_init(void);

extern volatile int line_ready;
extern char line_buff[];

static volatile unsigned char *video = (volatile unsigned char *)0xB8000;

static int cursor = 0;

void print(const char *str) {
    while (*str != '\0')
    {
        if (*str == '\n')
        {
            cursor = (cursor / VGA_WIDTH + 1 ) * VGA_WIDTH;
        }
        else if (*str == '\b')
        {
            if (cursor > 0)
            {
                cursor--;

                video[cursor * 2] = ' ';
                video[cursor * 2 + 1] = 0x07;
            }
        }
        else
        {
            video[cursor * 2] = *str;
            video[cursor * 2 + 1] = 0x07;

            cursor++;
        }

        str++;
    }
}

void kernel_main(void)
{
    print("WEE64 0.1.0\n");
    print("Welcome to wee64!\n");
    print("64bit kernel online.\n");

    idt_init();

    print("\nwee64> ");

    __asm__ volatile ("sti");

    while (1)
    {
        __asm__ volatile ("hlt" ::: "memory");

        if (line_ready)
        {
            print("you typed: ");
            print(line_buff);
            print("\n");

            line_ready = 0;
            print("wee64> ");
        }
    }
}
