#ifndef FLAGS_H
#define FLAGS_H

#include "procflags.h"
#include <stdio.h>
#include <string.h>

#define has_flag(flags, x) (flags & (1 << x) && 1)

struct u32_flags {
    int n;
    const char *name[32];
};

int print_flags(struct u32_flags tab, unsigned long flags, char *str, const char *delim)
{
    int num = 0, i = 0;
    str[0] = '\0';

    while (i < tab.n) {
        if (has_flag(flags, i) && tab.name[i]) {
            sprintf(str, "%s%s%s", str, (num ? delim : ""), tab.name[i]);
            num++;
        }
        i++;
    }
    return num;
}

struct u32_flags proc_flags = {
    .n = 32,
    .name = {
        NULL,
        NULL,
        "EXITING",
        "EXITPIDONE",
        "VCPU",
        "WQ_WORKER",
        "FORKNOEXEC",
        "MCE_PROCESS",
        "SUPERPRIV",
        "DUMPCORE",
        "SIGNALED",
        "MEMALLOC",
        "NPROC_EXCEEDED",
        "USED_MATH",
        "USED_ASYNC",
        "NOFREEZE",
        "FROZEN",
        "FSTRANS",
        "KSWAPD",
        "MEMALLOC_NOIO",
        "LESS_THROTTLE",
        "KTHREAD",
        "RANDOMIZE",
        "SWAPWRITE",
        NULL,
        NULL,
        "NO_SETAFFINITY",
        "MCE_EARLY",
        NULL,
        "MUTEX_TESTER",
        "FREEZER_SKIP",
        "SUSPEND_TASK",
    }
};

#endif // FLAGS_H
