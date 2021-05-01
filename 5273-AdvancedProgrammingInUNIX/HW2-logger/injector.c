#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/types.h>

#include "logger.h"

static atomic_bool initializing;
static atomic_bool initialized;

static int (*_chmod)(const char *path, mode_t mode) = NULL;
static int (*_chown)(const char *path, uid_t owner, gid_t group) = NULL;
static int (*_close)(int fildes) = NULL;
static int (*_creat)(const char *path, mode_t mode) = NULL;
static int (*_fclose)(FILE *stream) = NULL;
static FILE *(*_fopen)(const char *restrict pathname,
                       const char *restrict mode) = NULL;
static size_t (*_fread)(void *restrict ptr, size_t size, size_t nmemb,
                        FILE *restrict stream) = NULL;
static size_t (*_fwrite)(const void *restrict ptr, size_t size, size_t nmemb,
                         FILE *restrict stream) = NULL;
static int (*_open)(const char *path, int oflag, ...) = NULL;
static ssize_t (*_read)(int fildes, void *buf, size_t nbyte) = NULL;
static int (*_remove)(const char *pathname) = NULL;
static int (*_rename)(const char *old, const char *new) = NULL;
static FILE *(*_tmpfile)(void) = NULL;
static ssize_t (*_write)(int fildes, const void *buf, size_t nbyte) = NULL;

void injector_init()
{
    bool is_init = atomic_exchange(&initializing, true);
    if (is_init) {
        return;
    }

    _chmod = dlsym(RTLD_NEXT, "chmod");
    _chown = dlsym(RTLD_NEXT, "chown");
    _close = dlsym(RTLD_NEXT, "close");
    _creat = dlsym(RTLD_NEXT, "creat");
    _fclose = dlsym(RTLD_NEXT, "fclose");
    _fopen = dlsym(RTLD_NEXT, "fopen");
    _fread = dlsym(RTLD_NEXT, "fread");
    _fwrite = dlsym(RTLD_NEXT, "fwrite");
    _open = dlsym(RTLD_NEXT, "open");
    _read = dlsym(RTLD_NEXT, "read");
    _remove = dlsym(RTLD_NEXT, "remove");
    _rename = dlsym(RTLD_NEXT, "rename");
    _tmpfile = dlsym(RTLD_NEXT, "tmpfile");
    _write = dlsym(RTLD_NEXT, "write");

    atomic_store_explicit(&initialized, true, memory_order_release);
}

void wait_injector_init()
{
    bool is_init = atomic_load_explicit(&initializing, memory_order_acquire);
    if (!is_init) {
        injector_init();
    }

    while (!atomic_load_explicit(&initialized, memory_order_acquire));
}

/* --- Injection of main function --- */

typedef int (*libc_start_main_t)(int(int, char **, char **), int, char **,
                                 int(int, char **, char **), void(void),
                                 void(void), void *);

static int (*main_func)(int, char **, char **) = NULL;

int logger_main(int argc, char *argv[], char *envp[])
{
    logger_init();
    injector_init();

    return main_func(argc, argv, envp);
}

int __libc_start_main(int (*main)(int, char **, char **), int argc, char **argv,
                      int (*init)(int, char **, char **), void (*fini)(void),
                      void (*rtld_fini)(void), void *stack_end)
{
    main_func = main;

    libc_start_main_t orig_libc_main = dlsym(RTLD_NEXT, "__libc_start_main");

    return orig_libc_main(logger_main, argc, argv, init, fini, rtld_fini,
                          stack_end);
}

