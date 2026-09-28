extern void print(const char *str);
extern int strcmp(const char *a, const char *b);

typedef void (*cmd_fn)(int argc, int **argv);

typedef struct
{
    const char *name;
    const char *help;
    cmd_fn fn;
} command;

static void cmd_about(int argv, int **argc)
{
    (void)argv;
    (void)argc;

    print("wee64 0.1.0 - wee bit small, ain't it?\n");
}

static void cmd_echo(int argc, char **argv)
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
