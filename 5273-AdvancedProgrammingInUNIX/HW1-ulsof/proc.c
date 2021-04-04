#define _GNU_SOURCE
#include "proc.h"
#include "utils.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <unistd.h>

void proc_mkpath(char *s, size_t slen, pid_t pid, const char *path)
{
    if (path == NULL || strlen(path) == 0) {
        snprintf(s, sizeof(char) * slen, "/proc/%u", pid);
    } else {
        snprintf(s, sizeof(char) * slen, "/proc/%u/%s", pid, path);
    }
    s[slen - 1] = '\0';
}

int proc_read(pid_t pid, const char *path, char *buf, size_t buflen)
{
    char fullpath[PATH_MAX + 1];
    proc_mkpath(fullpath, PATH_MAX + 1, pid, path);

    struct stat st;
    if (lstat(fullpath, &st) < 0) {
        snprintf(buf, buflen, "%s (stat: %s)", fullpath, strerror(errno));
        buf[buflen - 1] = '\0';
        return -1;
    }

    if (S_ISREG(st.st_mode)) {
        FILE *fp = fopen(fullpath, "r");
        if (fp == NULL) {
            snprintf(buf, buflen, "%s (open: %s)", fullpath, strerror(errno));
            buf[buflen - 1] = '\0';
            return -1;
        }

        fgets(buf, buflen, fp);
        fclose(fp);

        size_t len = strlen(buf);
        if (buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }
    } else if ((S_ISLNK(st.st_mode))) {
        proc_readlink(pid, path, buf, buflen, NULL);
    }

    return 0;
}

int proc_readlink(pid_t pid, const char *path, char *buf, size_t buflen, struct stat *st)
{
    char fullpath[PATH_MAX + 1];
    proc_mkpath(fullpath, PATH_MAX + 1, pid, path);

    ssize_t lnklen;
    if ((lnklen = readlink(fullpath, buf, buflen)) < 0) {
        snprintf(buf, buflen, "%s (readlink: %s)", fullpath, strerror(errno));
        buf[buflen - 1] = '\0';
        return -1;
    }
    buf[MIN(lnklen, buflen - 1)] = '\0';

    if (st) {
        if (stat(fullpath, st) < 0) {
            return -1;
        }
    }
    return 0;
}

const static char *DELETED_POSTFIX = " (deleted)";

bool proc_isdeleted(const char *str)
{
    const size_t POSTFIX_LEN = strlen(DELETED_POSTFIX);

    if (!str) {
        return false;
    }

    size_t len = strlen(str);
    if (len < POSTFIX_LEN) {
        return false;
    }

    const char *p = str + (len - POSTFIX_LEN);
    return (!strcmp(p, DELETED_POSTFIX));
}

void proc_memmap_free(struct proc_memmap *m)
{
    struct proc_memmap *cur = m;
    while (cur != NULL) {
        struct proc_memmap *next = cur->next;

        free(cur->name);
        free(cur);
        cur = next;
    }
}

struct proc_memmap *proc_read_memmap(pid_t pid, const struct stat *txtstat)
{
    char fullpath[PATH_MAX + 1];
    proc_mkpath(fullpath, PATH_MAX + 1, pid, "maps");

    FILE *mapf = fopen(fullpath, "r");
    if (!mapf) {
        return NULL;
    }

    struct proc_memmap *map = NULL;

    char *linebuf = NULL;
    size_t buflen = 0;
    while (getline(&linebuf, &buflen, mapf) != EOF) {
        dev_t dev;
        unsigned dev_maj, dev_min;
        ino_t inode;
        // <start>-<end> <perms> <offset> <dev> <inode> <pathname>
        int scanlen;
        sscanf(linebuf, "%*x-%*x%*s%*x%x:%x%lu%n", &dev_maj, &dev_min, &inode, &scanlen);
        dev = makedev(dev_maj, dev_min);

        // Check if anonymous mapping
        if (dev == 0 || inode == 0) {
            continue;
        }

        char path[PATH_MAX + 1];
        strncpy(path, trim((char *)linebuf + scanlen), sizeof(path));

        // Check if match with the proc txt
        if (txtstat && (txtstat->st_dev == dev && txtstat->st_ino == inode)) {
            continue;
        }

        // Check not exist in any of entry
        struct proc_memmap *i, *last = NULL;
        bool existed = false;
        for (i = map; i != NULL; i = i->next) {
            if (i->mm_dev == dev && i->mm_ino == inode) {
                existed = true;
                break;
            }
            last = i;
        }
        if (existed) {
            continue;
        }

        if (last == NULL) {
            map = (struct proc_memmap *)malloc(sizeof(struct proc_memmap));
            if (!map) {
                return NULL;
            }
            last = map;
        } else {
            last->next = (struct proc_memmap *)malloc(sizeof(struct proc_memmap));
            if (!map) {
                return NULL;
            }
            last = last->next;
        }
        last->mm_dev = dev;
        last->mm_ino = inode;
        last->name = strdup(path);
        last->next = NULL;
    }

    free(linebuf);
    fclose(mapf);

    return map;
}

