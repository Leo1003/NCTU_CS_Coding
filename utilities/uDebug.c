#include <fcntl.h>
#include <linux/memfd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#define perrf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__);


int main(int argc, char *argv[])
{
    if (argc < 2) {
        perrf("Not providing the program name!\n");
        return 1;
    }
    int memfd = syscall(__NR_memfd_create, "", 0);
    if (memfd < 0) {
        perrf("Failed to create memfd!\n");
        return 1;
    }
    int wstatus;
    pid_t pid = fork();
    if (pid > 0) {
        if (waitpid(-1, &wstatus, 0) < 0) {
            kill(pid, SIGKILL);
            return 1;
        }
    } else if (pid == 0) {
        close(STDOUT_FILENO);
        if (dup2(memfd, STDOUT_FILENO) < 0) {
            perrf("Failed to redirect output!\n");
            exit(1);
        }
        close(memfd);
        close(STDERR_FILENO);
        if (open("/dev/null", O_WRONLY) < 0) {
            perrf("Failed to open \"/dev/null\"!\n");
            exit(1);
        }
        execv(argv[1], argv + 1);
        perrf("Couldn't execute the program: %s\n", argv[1]);
        exit(127);
    } else {
        perrf("Failed to create new process!\n");
        return 1;
    }
    if (!WIFEXITED(wstatus) || WEXITSTATUS(wstatus)) {
        perrf("Child process failed!\n");
        perrf("Exit status: %d\n", WEXITSTATUS(wstatus));
        return 1;
    }

    size_t size = lseek(memfd, 0, SEEK_END);
    if (size < 0) {
        perrf("Failed to get output size!\n");
        return 1;
    }
    void *output = mmap(NULL, size, PROT_READ, MAP_PRIVATE, memfd, 0);
    perrf("--------Program Output--------\n");
    fwrite(output, 1, size, stdout);
    printf("\n");
    return 0;
}

