#include "../fs/include/fat32.h"
#include <stdint.h>

extern void print(const char *str);
extern void print_color(const char *str, unsigned char color);
extern void print_color_n(const char *str, unsigned long len, unsigned char color);
extern int strcmp(const char *a, const char *b);
extern void clear_screen(void);
extern void diagnostics(void);
extern void poweroff(void);

extern const char _binary_assets_art_txt_start[];
extern const char _binary_assets_art_txt_end[]; 

extern fat32_fs_t filesystem;

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

    print_color_n(_binary_assets_art_txt_start, 
        _binary_assets_art_txt_end - _binary_assets_art_txt_start, 0x0A);
    
    print("\n");

    print_color("-------------------------------\n", 0x08);
    print_color("a lightweight open-source OS\n", 0x01);
    print_color("arch: x86_64\n", 0x0B);
    print_color("license: MIT\n", 0x0E);
    print_color("source: https://github.com/luuggelis/wee64\n", 0x07);
    print_color("\na bit wee, ain't it?\n", 0x08);
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

    print_color("wee64 v" WEE64_VERSION "\n", 0x0B);
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

static void cmd_exit(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    print_color("wee64 shutting down...\n", 0x0E);
    poweroff();
}

static void cmd_ls(int argc, char **argv)
{
    (void)argc;
    (void)argv;


    print("-------------\n");
    fat32_list_root(&filesystem);
    print("-------------\n");
}

static void cmd_cat(int argc, char **argv)
{
    if (argc < 2)
    {
        print("usage: cat <filename>");
        return;
    }

    uint8_t buffer[4097];

    uint8_t dir_buffer[4096];

    if (fat32_read_cluster(
            &filesystem,
            filesystem.root_cluster,
            dir_buffer,
            sizeof(dir_buffer)) != 0)
    {
        print_color("[ cat ] ", 0x0C);
        print("failed to read directory\n");
        return;
    }

    for (unsigned offset = 0;
        offset < filesystem.bytes_per_sector *
                 filesystem.sectors_per_cluster;
        offset += 32)  
    {
        uint8_t *entry = &dir_buffer[offset];

        if (entry[0] == 0x00)
            break;

        if (entry[0] == 0xE5 ||
            entry[11] == 0x0F ||
            (entry[11] & 0x18))
            continue;

        char name[13];
        unsigned n = 0;

        for (unsigned i = 0; i < 8 && entry[i] != ' '; i++)
            name[n++] = (char)entry[i];

        if (entry[8] != ' ')
        {
            name[n++] = '.';

            for (unsigned i = 8; i < 11 && entry[i] != ' '; i++)
                name[n++] = (char)entry[i];
        }

        name[n] = '\0';

        if (strcmp(argv[1], name) != 0)
            continue;

        uint32_t first_cluster =
            ((uint32_t)dir_buffer[offset + 20] |
             ((uint32_t)dir_buffer[offset + 21] << 8)) << 16;

        first_cluster |=
            (uint32_t)dir_buffer[offset + 26] |
            ((uint32_t)dir_buffer[offset + 27] << 8);

        uint32_t file_size =
            (uint32_t)dir_buffer[offset + 28] |
            ((uint32_t)dir_buffer[offset + 29] << 8) |
            ((uint32_t)dir_buffer[offset + 30] << 16) |
            ((uint32_t)dir_buffer[offset + 31] << 24);

        if (file_size > 4096)
        {
            print_color("[ cat ] ", 0x0C);
            print("file too large (max 4096 bytes)\n");
            return;
        }

        if (file_size == 0)
        {
            print("\n");
            return;
        }

        if (fat32_read_file(
                &filesystem,
                first_cluster,
                file_size,
                buffer,
                sizeof(buffer) - 1) != 0)
        {
            print_color("[ cat ] ", 0x0C);
            print("failed to read file\n");
            return;
        }

        buffer[file_size] = '\0';
        print((const char *)buffer);
        print("\n");
        return;
    }

    print_color("[ cat ] ", 0x0C);
    print("file not found\n");
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
    { "exit",     "shuts off wee64",         cmd_exit     },
    { "ls",       "lists files in dir",      cmd_ls       },
    { "cat",      "prints file contents",    cmd_cat      },
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

    print_color("unknown command: ", 0x0C);
    print(argv[0]);
    print("\n");
}
