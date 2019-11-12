#include "proc_stat.h"
#include "flags.h"
#define BUFFER 2048

void usage()
{
    errf("readstat <pid>");
}

#define GETUI(x) fscanf(fp, "%u", &(x))
#define GETUL(x) fscanf(fp, "%lu", &(x))
#define GETI(x) fscanf(fp, "%d", &(x))
#define GETL(x) fscanf(fp, "%ld", &(x))
#define GETLL(x) fscanf(fp, "%lld", &(x))
#define GETULL(x) fscanf(fp, "%llu", &(x))
#define GETSTR(x) fscanf(fp, "%s", (x))
#define GETCHAR(x) fscanf(fp, "%*[ \n\t]%c", &(x))
#define GETSKIP() fscanf(fp, "%*s")
#define PRINT_SIGS(flags)                        \
    do {                                         \
        i = 0;                                   \
        while (i < 31) {                         \
            printf("%1d", has_flag(flags, i++)); \
        }                                        \
        printf("\n");                            \
    } while (0)
void read_stat(struct proc_stat *st, FILE *fp)
{
    char state;
    st->comm = (char *)malloc(BUFFER);
    GETI(st->pid);
    GETSTR(st->comm);
    GETCHAR(state);
    GETI(st->ppid);
    GETI(st->pgrp);
    GETI(st->sid);
    GETL(st->tty_nr);
    GETI(st->tgpid);
    GETUL(st->flags);
    GETUL(st->minflt);
    GETUL(st->cminflt);
    GETUL(st->majflt);
    GETUL(st->cmajflt);
    GETUL(st->utime);
    GETUL(st->stime);
    GETUL(st->cutime);
    GETUL(st->cstime);
    GETL(st->priority);
    GETL(st->nice);
    GETL(st->num_threads);
    GETSKIP();
    GETULL(st->starttime);
    GETUL(st->vsize);
    GETUL(st->rss);
    GETUL(st->rsslim);
    GETUL(st->s_code);
    GETUL(st->e_code);
    GETUL(st->s_stack);
    GETUL(st->p_stack);
    GETUL(st->i_stack);
    GETUL(st->signal);
    GETUL(st->blocked);
    GETUL(st->sigignore);
    GETUL(st->sigcatch);
    GETUL(st->wchan);
    GETSKIP();
    GETSKIP();
    GETL(st->exit_signal);
    GETL(st->processor);
    GETUI(st->rt_priority);
    GETUI(st->policy);
    GETULL(st->delayacct_blkio_ticks);
    GETUL(st->guest_time);
    GETUL(st->cguest_time);
    GETUL(st->s_data);
    GETUL(st->e_data);
    GETUL(st->s_brk);
    GETUL(st->s_arg);
    GETUL(st->e_arg);
    GETUL(st->s_env);
    GETUL(st->e_env);
    GETL(st->exit_code);
    switch (state) {
        case 'R':
            st->state = ST_RUNNING;
            break;
        case 'S':
            st->state = ST_SLEEP;
            break;
        case 'D':
            st->state = ST_DISK_SLEEP;
            break;
        case 'Z':
            st->state = ST_ZOMBIE;
            break;
        case 'T':
            st->state = ST_STOPPED;
            break;
        default:
            st->state = ST_UNKNOWN;
            break;
    }
}

void print_stat(struct proc_stat st)
{
    char strflags[BUFFER];
    const long TICK = sysconf(_SC_CLK_TCK);
    int i;
    printf("PID: %d\t\tPPID: %d\t\tPGRP: %d\t\tSID: %d\t\tTGPID: %d\n", st.pid, st.ppid, st.pgrp, st.sid, st.tgpid);
    printf("Name: %s\n", st.comm);
    printf("State: ");
    switch (st.state) {
        case ST_RUNNING:
            printf("(R)Running");
            break;
        case ST_SLEEP:
            printf("(S)Sleep");
            break;
        case ST_DISK_SLEEP:
            printf("(D)DiskSleep");
            break;
        case ST_STOPPED:
            printf("(T)Traced");
            break;
        case ST_ZOMBIE:
            printf("(Z)Zombie");
            break;
        case ST_UNKNOWN:
            printf("(?)Unknown");
            break;
    }
    unsigned long long stsec = st.starttime / TICK;
    printf("\n");
    printf("TTY: %ld:%ld\n", MAJOR(st.tty_nr), MINOR(st.tty_nr));
    print_flags(proc_flags, st.flags, strflags, " | ");
    printf("Flags: %s\n", strflags);
    printf("MinorFaults: %lu\t\tMajorFaults: %lu\n", st.minflt, st.majflt);
    printf("ChildMinorFaults: %lu  \tChildMajorFaults: %lu\n", st.cminflt, st.cmajflt);
    printf("UserCPUTime: %lu.%lu\t\tSysCPUTime: %lu.%lu\n", st.utime / TICK, st.utime % TICK, st.stime / TICK, st.stime % TICK);
    printf("ChildUserCPUTime: %lu.%lu  \tChildSysCPUTime: %lu.%lu\n", st.cutime / TICK, st.cutime % TICK, st.cstime / TICK, st.cstime % TICK);
    printf("Priority: %ld\tNice: %ld\nThreads: %lu\n", st.priority, st.nice, st.num_threads);
    printf("RT_Proirity: %u\n", st.rt_priority);
    printf("StartAt: %02llu-%02llu:%02llu:%02llu\n", stsec / 86400, stsec % 86400 / 3600, stsec % 3600 / 60, stsec % 60);
    printf("Aggregated block I/O: %llu.%llu\n", st.delayacct_blkio_ticks / TICK, st.delayacct_blkio_ticks % TICK);
    printf("GuestTime: %lu.%lu\t\tChildGuestTime: %lu.%lu\n", st.guest_time / TICK, st.guest_time % TICK, st.cguest_time / TICK, st.cguest_time % TICK);
    printf("Processor: %ld\n", st.processor);
    printf("ExitSignal: %ld\n", st.exit_signal);
    printf("ExitCode: %ld\n", st.exit_code);
    printf("Virtual: %lu KB\n", st.vsize / 1024);
    printf("RSS/LIMIT: %lu/%lu KB\n", st.rss / 1024, (st.rsslim == __UINT64_MAX__ ? 0 : st.rsslim / 1024));
    printf("Policy: ");
    switch (st.policy % 8) {
        case SCHED_NORMAL:
            printf("Normal");
            break;
        case SCHED_FIFO:
            printf("FIFO");
            break;
        case SCHED_RR:
            printf("RR");
            break;
        case SCHED_BATCH:
            printf("Batch");
            break;
        case SCHED_IDLE:
            printf("Idle");
            break;
        case SCHED_DEADLINE:
            printf("Deadline");
            break;
        default:
            printf("Unknown");
            break;
    }
    if (st.policy & SCHED_FLAG_RESET_ON_FORK) printf(" | Reset_On_Fork");
    printf("\n");
    printf("Code: 0x%016lx ~ 0x%016lx\n", st.s_code, st.e_code);
    printf("Stack: 0x%016lx\n", st.s_stack);
    printf("StackPointer: 0x%016lx\n", st.p_stack);
    printf("EIP: 0x%016lx\n", st.i_stack);
    printf("Data: 0x%016lx ~ 0x%016lx\n", st.s_data, st.e_data);
    printf("BRK: 0x%016lx\n", st.s_brk);
    printf("Arg: 0x%016lx ~ 0x%016lx\n", st.s_arg, st.e_arg);
    printf("Env: 0x%016lx ~ 0x%016lx\n", st.s_env, st.e_env);
    printf("Wchan: 0x%016lx\n", st.wchan);
    printf("----------------Signals----------------\n");
    printf("\t 0        1         2         3 \n");
    printf("\t 1234567890123456789012345678901\n");
    printf("Pending: ");
    PRINT_SIGS(st.signal);
    printf("Blocked: ");
    PRINT_SIGS(st.blocked);
    printf("Ignore:  ");
    PRINT_SIGS(st.sigignore);
    printf("Catch:   ");
    PRINT_SIGS(st.sigcatch);
}


int main(int argc, char *argv[])
{
    if (argc != 2) {
        usage();
        exit(2);
    }
    pid_t pid = atoi(argv[1]);

    char target[BUFFER];
    sprintf(target, "/proc/%d/stat", pid);
    struct stat st;
    if (stat(target, &st) < 0) {
        errf("Process %d not found\n", pid);
        exit(1);
    }
    FILE *fp;
    fp = fopen(target, "r");
    if (!fp) {
        errf("Failed to open %s\n", target);
        exit(1);
    }
    struct proc_stat procst;
    read_stat(&procst, fp);
    print_stat(procst);
}
