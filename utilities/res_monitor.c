#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <seccomp.h>

#define errf(x, ...) fprintf(stderr, x, ##__VA_ARGS__)

bool chld_flag, int_flag, term_flag;

void syserr(const char *name)
{
    errf("System Call Error: %s  %s\n", name, strerror(errno));
    exit(1);
}

void usage()
{
    errf("res_monitor <program> [args, ...]\n");
}

void printprocinfo(struct rusage *rus, siginfo_t *infop)
{
    int exittype = 0;
    printf("\n");
    printf("--------Process status--------\n");
    printf("PID: %d\n", infop->si_pid);
    printf("USER: %d\n", infop->si_uid);

    switch (infop->si_code) {
        case CLD_EXITED:
            printf("STATUS: EXITED\n");
            exittype = 0;
            break;
        case CLD_KILLED:
            printf("STATUS: KILLED\n");
            exittype = 1;
            break;
        case CLD_DUMPED:
            printf("STATUS: COREDUMP\n");
            exittype = 1;
            break;
    }
    if (exittype) {
        printf("SIGNAL: %d\n", infop->si_status);
        if (infop->si_status == SIGSYS) {
            printf("SYSCALL: %s\n", seccomp_syscall_resolve_num_arch(seccomp_arch_native(), infop->si_syscall));
            printf("ARCH: %d\n", infop->si_arch);
            printf("CALL_ADDR: %p\n", infop->si_call_addr);
        }
    } else {
        printf("EXITCODE: %d\n", infop->si_status);
    }

    printf("--------Resource Usage--------\n");
    printf("USER_CPU: %ld.%06ld\n", rus->ru_utime.tv_sec, rus->ru_utime.tv_usec);
    printf("SYS_CPU: %ld.%06ld\n", rus->ru_stime.tv_sec, rus->ru_stime.tv_usec);
    printf("RSS: %ld\n", rus->ru_maxrss);
    //printf("SHARED: %ld\n", rus->ru_ixrss);
    //printf("DATA: %ld\n", rus->ru_idrss);
    //printf("STACK: %ld\n", rus->ru_isrss);
    printf("PAGE_RECLAIM: %ld\n", rus->ru_minflt);
    printf("PAGE_FAULT: %ld\n", rus->ru_majflt);
    //printf("SWAP: %ld\n", rus->ru_nswap);
    printf("BLOCK_INPUT: %ld\n", rus->ru_inblock);
    printf("BLOCK_OUTPUT: %ld\n", rus->ru_oublock);
    //printf("IPC_SEND: %ld\n", rus->ru_msgsnd);
    //printf("IPC_RECV: %ld\n", rus->ru_msgrcv);
    //printf("SIGNALS: %ld\n", rus->ru_nsignals);
    printf("VCSW: %ld\n", rus->ru_nvcsw);
    printf("IVCSW: %ld\n", rus->ru_nivcsw);
}

void sig_handler(int sig, siginfo_t *siginfo, void *data)
{
    switch (sig) {
        case SIGINT:
            int_flag = true;
        case SIGTERM:
            term_flag = true;
        case SIGCHLD:
            chld_flag = true;
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        usage();
        exit(2);
    }
    pid_t chpid = fork();
    if (chpid == 0) {
        execvp(argv[1], argv + 1);
        errf("Execve Error: %s\n", strerror(errno));
        exit(127);
    } else if (chpid > 0) {
        printf("Child PID: %d\n", chpid);
        struct sigaction sa;
        sa.sa_sigaction = sig_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = SA_RESTART | SA_SIGINFO | SA_NOCLDSTOP;
        if (sigaction(SIGINT, &sa, NULL) < 0)
            syserr("sigaction");
        if (sigaction(SIGCHLD, &sa, NULL) < 0)
            syserr("sigaction");
        if (sigaction(SIGTERM, &sa, NULL) < 0)
            syserr("sigaction");

        if (!(chld_flag || int_flag || term_flag))
            pause();

        if (int_flag || term_flag) {
            //if(kill(-chpid, SIGKILL) < 0)
            //    syserr("kill");
        }
        pause();
        struct rusage rus;
        siginfo_t infop;
        if (waitid(P_ALL, 0, &infop, WEXITED) < 0)
            syserr("waitid");
        if (getrusage(RUSAGE_CHILDREN, &rus) < 0)
            syserr("getrusage");
        if (!infop.si_pid) {
            errf("Failed to find child process\n");
            exit(1);
        }
        if (infop.si_code == CLD_EXITED && infop.si_status == 127)
            exit(127);
        printprocinfo(&rus, &infop);
    } else {
        syserr("fork");
    }
}
