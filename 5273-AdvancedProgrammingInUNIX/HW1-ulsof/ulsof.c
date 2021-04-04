#define _GNU_SOURCE
#include "ulsof.h"

#include "proc.h"
#include "utils.h"
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

const char *enty_str(enum entry_type type)
{
    switch (type) {
    case ENTRY_TYPE_NONE:
        return "";
    case ENTRY_TYPE_DIR:
        return "DIR";
    case ENTRY_TYPE_REG:
        return "REG";
    case ENTRY_TYPE_CHR:
        return "CHR";
    case ENTRY_TYPE_FIFO:
        return "FIFO";
    case ENTRY_TYPE_SOCK:
        return "SOCK";
    case ENTRY_TYPE_UNKNOWN:
        return "unknown";
    default:
        return NULL;
    }
}

static inline int tab_calc(size_t slen, size_t maxtab)
{
    int tabs = maxtab - (slen / 8);
    if (tabs < 1) {
        tabs = 1;
    }
    return tabs;
}
static inline void print_nchr(char c, size_t n) {
    while (n--) {
        putchar(c);
    }
}

enum entry_type enty_from_mode(mode_t mode)
{
    switch (mode & S_IFMT) {
    case S_IFSOCK:
        return ENTRY_TYPE_SOCK;
    case S_IFREG:
        return ENTRY_TYPE_REG;
    case S_IFDIR:
        return ENTRY_TYPE_DIR;
    case S_IFCHR:
        return ENTRY_TYPE_CHR;
    case S_IFIFO:
        return ENTRY_TYPE_FIFO;
    default:
        return ENTRY_TYPE_UNKNOWN;
    }
}

void print_entry(const char* comm, pid_t pid, const char *user, int fd, unsigned flag,
                 enum entry_type type, ino_t inode, const char *file)
{
    if (opt_type_filter != ENTRY_TYPE_NONE) {
        if (opt_type_filter != type) {
            return;
        }
    }

    if (opt_file && file != NULL) {
        if (regexec(&opt_file_filter, file, 0, NULL, 0)) {
            return;
        }
    }

    // Print process info
    int comm_ntab = tab_calc(strlen(comm), 5);
    int user_ntab = tab_calc(strlen(user), 2);
    printf("%s", comm);
    print_nchr('\t', comm_ntab);
    printf("%u\t%s", pid, user);
    print_nchr('\t', user_ntab);

    char fdstrbuf[256];
    const char* fdstr = "?";
    if (fd >= 0) {
        char fd_mode = 'x';
        if ((flag & ENTRY_FLAG_RD) && (flag & ENTRY_FLAG_WR)) {
            fd_mode = 'u';
        } else if (flag & ENTRY_FLAG_RD) {
            fd_mode = 'r';
        } else if (flag & ENTRY_FLAG_WR) {
            fd_mode = 'w';
        }
        snprintf(fdstrbuf, sizeof(fdstrbuf), "%d%c", fd, fd_mode);
        fdstr = fdstrbuf;
    } else {
        switch (fd) {
        case ENTRY_FD_CWD:
            fdstr = "cwd";
            break;
        case ENTRY_FD_ROOT:
            fdstr = "root";
            break;
        case ENTRY_FD_EXE:
            fdstr = "exe";
            break;
        case ENTRY_FD_MEM:
            fdstr = "mem";
            break;
        case ENTRY_FD_DEL:
            fdstr = "del";
            break;
        case ENTRY_FD_NOFD:
            fdstr = "NOFD";
            break;
        }
    }

    printf("%s\t%s\t", fdstr, enty_str(type));
    if (flag & ENTRY_FLAG_ER || inode == 0) {
        printf("\t%s\n", file);
    } else {
        printf("%lu\t%s\n", inode, file);
    }
}

int proc_iter()
{
    printf("COMMAND\t\t\t\t\tPID\tUSER\t\tFD\tTYPE\tNODE\tNAME\n");

    DIR *procdir = opendir("/proc");
    if (procdir == NULL) {
        return -1;
    }

    struct dirent *entry = NULL;
    while ((entry = readdir(procdir)) != NULL) {
        size_t len = strlen(entry->d_name);
        bool valid = true;
        for (size_t i = 0; i < len; i++) {
            if (!(entry->d_name[i] >= '0' && entry->d_name[i] <= '9')) {
                valid = false;
                break;
            }
        }
        if (!valid) {
            continue;
        }

        pid_t pid = (pid_t)atoi(entry->d_name);
        get_proc_info(pid);
    }

    closedir(procdir);

    return 0;
}

int get_proc_info(pid_t pid)
{
    char dirpath[PATH_MAX + 1];
    proc_mkpath(dirpath, PATH_MAX + 1, pid, "");

    struct stat st;
    if (stat(dirpath, &st) < 0) {
        return -1;
    }
    if (!S_ISDIR(st.st_mode)) {
        return 0;
    }

    char command[256 + 1];
    if (proc_read(pid, "comm", command, sizeof(command))) {
        return -1;
    }
    // filter commandline
    if (opt_cmd) {
        if (regexec(&opt_cmd_filter, command, 0, NULL, 0)) {
            return 0;
        }
    }

    // Resolve user
    char username[256 + 1];
    struct passwd *pwd = NULL;
    if ((pwd = getpwuid(st.st_uid)) == NULL) {
        snprintf(username, sizeof(username), "%u", st.st_uid);
    } else {
        strncpy(username, pwd->pw_name, sizeof(username));
    }
    username[256] = '\0';

    char buf[PATH_MAX + 1];
    struct stat stlnk, txtstat;
    // cwd
    if (proc_readlink(pid, "cwd", buf, sizeof(buf), &stlnk) < 0) {
        // error
        print_entry(command, pid, username, ENTRY_FD_CWD, ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
    } else {
        print_entry(command, pid, username, ENTRY_FD_CWD, 0, enty_from_mode(stlnk.st_mode), stlnk.st_ino, buf);
    }
    // root
    if (proc_readlink(pid, "root", buf, sizeof(buf), &stlnk) < 0) {
        // error
        print_entry(command, pid, username, ENTRY_FD_ROOT, ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
    } else {
        print_entry(command, pid, username, ENTRY_FD_ROOT, 0, enty_from_mode(stlnk.st_mode), stlnk.st_ino, buf);
    }
    // exe
    if (proc_readlink(pid, "exe", buf, sizeof(buf), &stlnk) < 0) {
        // error
        print_entry(command, pid, username, ENTRY_FD_EXE, ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
    } else {
        print_entry(command, pid, username, ENTRY_FD_EXE, 0, enty_from_mode(stlnk.st_mode), stlnk.st_ino, buf);
    }
    txtstat = stlnk;
    // mem
    struct proc_memmap *maps = proc_read_memmap(pid, &txtstat);
    if (maps) {
        struct proc_memmap *i;
        for (i = maps; i != NULL; i = i->next) {
            if (proc_isdeleted(i->name)) {
                print_entry(command, pid, username, ENTRY_FD_DEL, 0, ENTRY_TYPE_REG, i->mm_ino, i->name);
            } else {
                print_entry(command, pid, username, ENTRY_FD_MEM, 0, ENTRY_TYPE_REG, i->mm_ino, i->name);
            }
        }
    }
    proc_memmap_free(maps);
    // fd
    char fdpath[PATH_MAX + 1];
    proc_mkpath(fdpath, sizeof(fdpath), pid, "fd");
    DIR *fddir = opendir(fdpath);
    if (fddir == NULL) {
        snprintf(buf, sizeof(buf), "%s (opendir: %s)", fdpath, strerror(errno));
        print_entry(command, pid, username, ENTRY_FD_NOFD, ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
    } else {
        struct dirent *entry = NULL;
        int dfd = dirfd(fddir);

        while ((entry = readdir(fddir)) != NULL) {
            if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                continue;
            }
            int fd = atoi(entry->d_name);

            struct stat fdst, nodest;
            unsigned flag = 0;
            if (fstatat(dfd, entry->d_name, &fdst, AT_SYMLINK_NOFOLLOW) < 0) {
                // error
                snprintf(buf, sizeof(buf), "%s/%s (fstatat: %s)", fdpath, entry->d_name, strerror(errno));
                print_entry(command, pid, username, fd, ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
                continue;
            }
            if (fdst.st_mode & S_IRUSR) {
                flag |= ENTRY_FLAG_RD;
            }
            if (fdst.st_mode & S_IWUSR) {
                flag |= ENTRY_FLAG_WR;
            }

            ssize_t lnklen = 0;
            if ((lnklen = readlinkat(dfd, entry->d_name, buf, sizeof(buf))) < 0) {
                // error
                snprintf(buf, sizeof(buf), "%s/%s (readlink: %s)", fdpath, entry->d_name, strerror(errno));
                print_entry(command, pid, username, fd, flag | ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
                continue;
            }
            buf[MIN(lnklen, sizeof(buf) - 1)] = '\0';

            if (fstatat(dfd, entry->d_name, &nodest, 0) < 0) {
                // error
                snprintf(buf, sizeof(buf), "%s/%s (fstatat: %s)", fdpath, entry->d_name, strerror(errno));
                print_entry(command, pid, username, fd, flag | ENTRY_FLAG_ER, ENTRY_TYPE_UNKNOWN, 0, buf);
                continue;
            }

            print_entry(command, pid, username, fd, flag, enty_from_mode(nodest.st_mode), nodest.st_ino, buf);
        }
        closedir(fddir);
    }

    return 0;
}

