#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

extern void idt_init(void);
extern int strcmp(const char *a, const char *b);

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

static int tokenize(char *line, char **argv, int max)
{
    int argc = 0;

    while (*line != '\0' && argc < max)
    {
        while (*line == ' ')
        {
            *line = '\0';
            line++;
        }

        if (*line != '\0')
        {
            argv[argc] = line;
            argc++;
        }

        while (*line != '\0' && *line != ' ')
        {
            line++;
        }
    }

    return argc;
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
            char *argv[16];
            int argc = tokenize(line_buff, argv, 16);

            if (argc > 0)
            {
                if (strcmp(argv[0], "help") == 0)
                {
                    print("this is the help command!\n");
                }
                else if (strcmp(argv[0], "about") == 0)
                {
                    print("wee64 0.1.0\n");
                }
                else if (strcmp(argv[0], "echo") == 0)
                {
                    for (int i = 1; i < argc; i++)
                    {
                        print(argv[i]);

                        if (i < argc - 1)
                        {
                            print(" ");
                        }
                    }

                    print("\n");
                }
                else
                {
                    print("unknown command: ");
                    print(argv[0]);
                    print("\n");
                }
            }

            line_ready = 0;
            print("wee64> ");
        }
    }
}
