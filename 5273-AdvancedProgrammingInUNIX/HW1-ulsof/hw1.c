#define _GNU_SOURCE
#include "ulsof.h"

#include <errno.h>
#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

bool opt_cmd = false;
regex_t opt_cmd_filter;
bool opt_file = false;
regex_t opt_file_filter;
enum entry_type opt_type_filter = ENTRY_TYPE_NONE;

int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "c:t:f:")) != -1) {
        int regerr = 0;
        char regmsg[1024];

        switch (opt) {
        case 'c':
            if ((regerr = regcomp(&opt_cmd_filter, optarg, REG_NOSUB)) != 0) {
                regerror(regerr, &opt_cmd_filter, regmsg, sizeof(regmsg));
                printf("Invalid regex: %s\n", regmsg);
                return 1;
            }
            opt_cmd = true;
            break;
        case 't':
            if (!strcmp(optarg, "REG")) {
                opt_type_filter = ENTRY_TYPE_REG;
            } else if (!strcmp(optarg, "CHR")) {
                opt_type_filter = ENTRY_TYPE_CHR;
            } else if (!strcmp(optarg, "DIR")) {
                opt_type_filter = ENTRY_TYPE_DIR;
            } else if (!strcmp(optarg, "FIFO")) {
                opt_type_filter = ENTRY_TYPE_FIFO;
            } else if (!strcmp(optarg, "SOCK")) {
                opt_type_filter = ENTRY_TYPE_SOCK;
            } else if (!strcmp(optarg, "unknown")) {
                opt_type_filter = ENTRY_TYPE_UNKNOWN;
            } else {
                printf("Invalid TYPE option.\n");
                return 1;
            }
            break;
        case 'f':
            if ((regerr = regcomp(&opt_file_filter, optarg, REG_NOSUB)) != 0) {
                regerror(regerr, &opt_file_filter, regmsg, sizeof(regmsg));
                printf("Invalid regex: %s\n", regmsg);
                return 1;
            }
            opt_file = true;
            break;
        default:
            break;
        }
    }

    proc_iter();
    return 0;
}

