void kernel_main(void)
{
    volatile char *video = (volatile char *)0xB8000;

    video[0] = 'W';
    video[1] = 0x07;

    video[2] = 'E';
    video[3] = 0x07;

    video[4] = 'E';
    video[5] = 0x07;

    video[6] = '6';
    video[7] = 0x07;

    video[8] = '4';
    video[9] = 0x07;
}
