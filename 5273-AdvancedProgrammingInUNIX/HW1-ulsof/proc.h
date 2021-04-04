#ifndef _PROC_H
#define _PROC_H

#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

struct proc_memmap {
    dev_t mm_dev;
    ino_t mm_ino;
    char *name;

    struct proc_memmap *next;
};

void proc_mkpath(char *s, size_t slen, pid_t pid, const char *path);

int proc_read(pid_t pid, const char *path, char *buf, size_t buflen);

int proc_readlink(pid_t pid, const char *path, char *buf, size_t buflen, struct stat *st);

bool proc_isdeleted(const char *str);

void proc_memmap_free(struct proc_memmap *m);

struct proc_memmap *proc_read_memmap(pid_t pid, const struct stat *txtstat);

#endif

