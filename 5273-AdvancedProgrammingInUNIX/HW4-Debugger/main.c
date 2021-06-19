#define _GNU_SOURCE
#include "debugger.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PROMPT "sdb> "
#define DELIM " \t\v\n"
#define MAX_ARGV 256

void usage()
{
    printf("usage: ./hw4 [-s script] [program]\n");
}

int process_cmdline(dbg_ctx *ctx, char *line);

int main(int argc, char *argv[])
{
    const char *scriptfile = NULL;
    const char *preloadfile = NULL;
    int opt;

    while ((opt = getopt(argc, argv, "s:")) != -1) {
        switch (opt) {
        case 's':
            scriptfile = optarg;
            break;
        default:
            usage();
            exit(1);
            break;
        }
    }

    if (optind < argc) { preloadfile = argv[optind]; }

    FILE *sfp = NULL;
    dbg_ctx debugger;
    dbg_ctx_init(&debugger);

    if (scriptfile) {
        sfp = fopen(scriptfile, "r");
        if (!sfp) {
            printf(LOGD "Failed to open script file: %s\n", scriptfile);
            exit(1);
        }
    }

    if (preloadfile) {
        if (dbg_cmd_load(&debugger, preloadfile) < 0) { }
    }

    char *line = NULL;
    size_t linez = 0;

    if (sfp) {
        while (getline(&line, &linez, sfp) != EOF) {
            process_cmdline(&debugger, line);
        }
    } else {
        printf(PROMPT);
        while (getline(&line, &linez, stdin) != EOF) {
            process_cmdline(&debugger, line);
            printf(PROMPT);
        }
    }

    dbg_shutdown(&debugger);

    return 0;
}

int parse_address(const char *s, unsigned long *num)
{
    char *endptr = NULL;

    if (s == NULL) {
        printf(LOGD "No address is given.\n");
        return -1;
    }
    *num = strtoul(s, &endptr, 0);
    if (endptr == s || *endptr != '\0') {
        printf(LOGD "Invalid address.\n");
        return -1;
    }

    return 0;
}

int parse_int(const char *s, long *num)
{
    char *endptr = NULL;

    if (s == NULL) {
        printf(LOGD "No argument is given.\n");
        return -1;
    }
    *num = strtol(s, &endptr, 0);
    if (endptr == s || *endptr != '\0') {
        printf(LOGD "Invalid integer.\n");
        return -1;
    }

    return 0;
}

int parse_value(const char *s, unsigned long *val)
{
    char *endptr = NULL;

    if (s == NULL) {
        printf(LOGD "No value is given.\n");
        return -1;
    }
    *val = strtol(s, &endptr, 0);
    if (*val == LONG_MAX && errno == ERANGE) {
        *val = strtoul(s, &endptr, 0);
    }
    if (endptr == s || *endptr != '\0') {
        printf(LOGD "Invalid value.\n");
        return -1;
    }

    return 0;
}

int process_cmdline(dbg_ctx *ctx, char *line)
{
    char *t = strtok(line, DELIM);

    if (strcmp(t, "break") == 0 || strcmp(t, "b") == 0) {
        t = strtok(NULL, DELIM);
        unsigned long addr;
        if (parse_address(t, &addr) < 0) {
            return -2;
        }

        return dbg_cmd_break(ctx, addr);
    } else if (strcmp(t, "cont") == 0 || strcmp(t, "c") == 0) {
        return dbg_cmd_cont(ctx);
    } else if (strcmp(t, "delete") == 0) {
        t = strtok(NULL, DELIM);
        long id;
        if (parse_int(t, &id) < 0) {
            return -2;
        }

        return dbg_cmd_delete(ctx, id);
    } else if (strcmp(t, "disasm") == 0 || strcmp(t, "d") == 0) {
        t = strtok(NULL, DELIM);
        unsigned long addr;
        if (t == NULL) {
            addr = 0UL;
        } else {
            if (parse_address(t, &addr) < 0) {
                return -2;
            }
        }

        return dbg_cmd_disasm(ctx, addr);
    } else if (strcmp(t, "dump") == 0 || strcmp(t, "x") == 0) {
        t = strtok(NULL, DELIM);
        unsigned long addr;
        if (t == NULL) {
            addr = 0UL;
        } else {
            if (parse_address(t, &addr) < 0) {
                return -2;
            }
        }

        return dbg_cmd_dump(ctx, addr);
    } else if (strcmp(t, "exit") == 0 || strcmp(t, "q") == 0) {
        return dbg_cmd_exit(ctx);
    } else if (strcmp(t, "get") == 0 || strcmp(t, "g") == 0) {
        char *reg = strtok(NULL, DELIM);
        if (reg == NULL) {
            printf(LOGD "No register is given.\n");
            return -2;
        }

        return dbg_cmd_get(ctx, reg);
    } else if (strcmp(t, "getregs") == 0) {
        return dbg_cmd_getregs(ctx);
    } else if (strcmp(t, "help") == 0 || strcmp(t, "h") == 0) {
        return dbg_cmd_help(ctx);
    } else if (strcmp(t, "list") == 0 || strcmp(t, "l") == 0) {
        return dbg_cmd_list(ctx);
    } else if (strcmp(t, "load") == 0) {
        char *file = strtok(NULL, DELIM);
        if (file == NULL) {
            printf(LOGD "No file is given.\n");
            return -2;
        }

        return dbg_cmd_get(ctx, file);
    } else if (strcmp(t, "run") == 0 || strcmp(t, "r") == 0) {
        char *argv[MAX_ARGV + 1];
        for (size_t i = 0; i < MAX_ARGV; i++) {
            argv[i] = strtok(NULL, DELIM);
            if (argv[i] == NULL) {
                break;
            }
        }
        argv[MAX_ARGV] = NULL;

        return dbg_cmd_run(ctx, (const char **)argv);
    } else if (strcmp(t, "vmmap") == 0 || strcmp(t, "m") == 0) {
        return dbg_cmd_vmmap(ctx);
    } else if (strcmp(t, "set") == 0 || strcmp(t, "s") == 0) {
        char *reg = strtok(NULL, DELIM);
        if (reg == NULL) {
            printf(LOGD "No register is given.\n");
            return -2;
        }

        unsigned long data;
        char *v = strtok(NULL, DELIM);
        if (parse_value(v, &data) < 0) {
            return -2;
        }

        return dbg_cmd_set(ctx, reg, data);
    } else if (strcmp(t, "si") == 0) {
        return dbg_cmd_si(ctx);
    } else if (strcmp(t, "start") == 0) {
        char *argv[MAX_ARGV + 1];
        for (size_t i = 0; i < MAX_ARGV; i++) {
            argv[i] = strtok(NULL, DELIM);
            if (argv[i] == NULL) {
                break;
            }
        }
        argv[MAX_ARGV] = NULL;

        return dbg_cmd_start(ctx, (const char **)argv);
    }

    return -3;
}

