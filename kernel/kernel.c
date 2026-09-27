#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile unsigned char *video = (volatile unsigned char *)0xB8000;

static int cursor = 0;

void print(const char *str) {
    while (*str != '\0')
    {
        if (*str == '\n')
        {
            cursor = (cursor / VGA_WIDTH + 1 ) * VGA_WIDTH;
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
    print("WEE64\n");
    print("Welcome to wee64!\n");
    print("64bit kernel online.\n");
}
