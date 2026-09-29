#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern void idt_init(void);
extern int strcmp(const char *a, const char *b);
extern void shell_execute(char *line);

extern volatile int line_ready;
extern char line_buff[];

static volatile unsigned char *video = (volatile unsigned char *)0xB8000;

static int cursor = 0;

static void scroll(void)
{
    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1) * 2; i++)
    {
        video[i] = video[i + VGA_WIDTH * 2];
    }

    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1) * 2; i < VGA_WIDTH * VGA_HEIGHT * 2; i += 2)
    {
        video[i] = ' ';
        video[i + 1] = 0x07;
    }

    cursor -= VGA_WIDTH;
}

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

        if (cursor >= VGA_WIDTH * VGA_HEIGHT)
        {
            scroll();
        }

        str++;
    }
}

void clear_screen(void)
{
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
    {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x07;
    }

    cursor = 0;
}

void kernel_main(void)
{
    print("WEE64 " WEE64_VERSION "\n");
    print("welcome to wee64!\n");

    idt_init(); 

    print("\nwee64> ");

    __asm__ volatile ("sti");

    while (1)
    {
        __asm__ volatile ("hlt" ::: "memory");

        if (line_ready)
        {
            shell_execute(line_buff);

            line_ready = 0;
            print("wee64> ");
        }
    }
}
