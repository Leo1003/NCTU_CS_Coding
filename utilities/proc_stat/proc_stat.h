#ifndef PROC_STAT_H
#define PROC_STAT_H

#include <errno.h>
#include <linux/kdev_t.h>
#include <linux/sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define errf(x, ...) fprintf(stderr, x, ##__VA_ARGS__)

enum e_state {
    ST_UNKNOWN = 0,
    ST_RUNNING,
    ST_SLEEP,
    ST_DISK_SLEEP,
    ST_ZOMBIE,
    ST_STOPPED
};
typedef unsigned long bitmap_t;
typedef unsigned long pointer_t;

struct proc_stat {
    pid_t pid, ppid, pgrp, sid, tgpid;
    char *comm;
    enum e_state state;
    long tty_nr;
    unsigned long flags, minflt, cminflt, majflt, cmajflt;
    //can be replace by struct tms
    unsigned long utime, stime, cutime, cstime;
    long priority, nice, num_threads;
    unsigned long long starttime, delayacct_blkio_ticks;
    unsigned long vsize, rss, rsslim;
    pointer_t s_code, e_code, s_stack, p_stack, i_stack, wchan, s_data, e_data, s_brk, s_arg, e_arg, s_env, e_env;
    bitmap_t signal, blocked, sigignore, sigcatch;
    long exit_signal, processor, exit_code;
    unsigned rt_priority, policy;
    unsigned long guest_time, cguest_time;
};

#endif
