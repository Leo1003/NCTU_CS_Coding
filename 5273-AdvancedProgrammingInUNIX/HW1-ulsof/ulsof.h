#ifndef _ULSOF_H
#define _ULSOF_H

#include <stdbool.h>
#include <sys/types.h>
#include <regex.h>

#define ENTRY_FD_CWD    (-1)
#define ENTRY_FD_ROOT   (-2)
#define ENTRY_FD_EXE    (-3)
#define ENTRY_FD_MEM    (-4)
#define ENTRY_FD_DEL    (-5)
#define ENTRY_FD_NOFD   (-16)

#define ENTRY_FLAG_RD   (0x00000001)
#define ENTRY_FLAG_WR   (0x00000002)
#define ENTRY_FLAG_RW   (0x00000003)
#define ENTRY_FLAG_ER   (0x80000000)

enum entry_type {
    ENTRY_TYPE_NONE = 0,
    ENTRY_TYPE_DIR,
    ENTRY_TYPE_REG,
    ENTRY_TYPE_CHR,
    ENTRY_TYPE_FIFO,
    ENTRY_TYPE_SOCK,
    ENTRY_TYPE_UNKNOWN,
};

const char *enty_str(enum entry_type type);
enum entry_type enty_from_mode(mode_t mode);

extern bool opt_cmd;
extern regex_t opt_cmd_filter;
extern bool opt_file;
extern regex_t opt_file_filter;
extern enum entry_type opt_type_filter;

int proc_iter();
int get_proc_info(pid_t pid);

#endif

