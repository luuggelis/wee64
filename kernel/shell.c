extern void print(const char *str);
extern int strcmp(const char *a, const char *b);
extern void clear_screen(void);
extern void diagnostics(void);

#define HOSTNAME "root"

typedef void (*cmd_fn)(int argc, char **argv);

typedef struct
{
    const char *name;
    const char *help;
    cmd_fn fn;
} command_t;

static void cmd_help(int argc, char **argv);

static void cmd_about(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print(
        "  _       __  _____  _____ \n"
        " | |     / / | ____|| ____|\n"
        " | | /| / /  |  _|  |  _|  \n"
        " | |/ |/ /   | |___ | |___ \n"
        " |__/|__/    |_____||_____|\n"
        " \n"
        "           WEE64\n"
        "  wee bit small, ain't it?\n"
        "a tiny open-source x86_64 OS\n"
        "\n"
    );
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

static void cmd_version(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print("wee64 v" WEE64_VERSION "\n");
}

static void cmd_dia(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    diagnostics();
}

static void cmd_hostname(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print("host: " HOSTNAME "\n");
}

static void cmd_clear(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    clear_screen();
}

static const command_t commands[] =
{
    { "help",     "list all commands",       cmd_help     },
    { "about",    "about wee64",             cmd_about    },
    { "echo",     "print arguments",         cmd_echo     },
    { "version",  "prints current version",  cmd_version  },
    { "hostname", "prints host name",        cmd_hostname },
    { "clear",    "clears terminal",         cmd_clear    },
    { "dia",      "show system diagnostics", cmd_dia      },
};

#define NUM_COMMANDS (sizeof(commands) / sizeof(commands[0]))

static void cmd_help(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    for (unsigned i = 0; i < NUM_COMMANDS; i++)
    {
        print(commands[i].name);
        print(" - ");
        print(commands[i].help);
        print("\n");
    }
}

static int tokenize(char *line, char **argv, int max)
{
    int argc = 0;

    while (*line != '\0' && argc < max)
    {
        // skip spaces, turning them into terminators
        while (*line == ' ')
        {
            *line = '\0';
            line++;
        }

        // if were now at a word, record its start
        if (*line != '\0')
        {
            argv[argc] = line;
            argc++;
        }

        // skip to the end of the word
        while (*line != '\0' && *line != ' ')
        {
            line++;
        }
    }

    return argc;
}

void shell_execute(char *line)
{
    char *argv[16];
    int argc = tokenize(line, argv, 16);

    if (argc == 0)
    {
        return;
    }

    for (unsigned i = 0; i < NUM_COMMANDS; i++)
    {
        if (strcmp(argv[0], commands[i].name) == 0)
        {
            commands[i].fn(argc, argv);
            return;
        }
    }

    print("unknown command: ");
    print(argv[0]);
    print("\n");
}
