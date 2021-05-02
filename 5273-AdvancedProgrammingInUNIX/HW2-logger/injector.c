#define _GNU_SOURCE
#include <dlfcn.h>
#include <limits.h>
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
static int (*_open)(const char *path, int oflag, mode_t mode) = NULL;
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
    injector_init();
    logger_init();
#ifndef NDEBUG
    logger_printf("[logger] Initialize completed!\n");
#endif

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

/* --- Injection of other functions --- */

int chmod(const char *path, mode_t mode)
{
    char pathbuf[PATH_MAX];

    if (_chmod == NULL) {
        wait_injector_init();
    }
    int ret = _chmod(path, mode);

    logger_fmt_path(pathbuf, sizeof(pathbuf), path);
    logger_printf(LOGGER_HEADER);
    logger_printf("chmod(\"%s\", %o)", pathbuf, mode);
    logger_printf(" = %d\n", ret);
    return ret;
}

int chown(const char *path, uid_t owner, gid_t group)
{
    char pathbuf[PATH_MAX];

    if (_chown == NULL) {
        wait_injector_init();
    }
    int ret = _chown(path, owner, group);

    logger_fmt_path(pathbuf, sizeof(pathbuf), path);
    logger_printf(LOGGER_HEADER);
    logger_printf("chown(\"%s\", %u, %u)", pathbuf, owner, group);
    logger_printf(" = %d\n", ret);
    return ret;
}

int close(int fildes)
{
    char pathbuf[PATH_MAX];

    logger_fmt_fd(pathbuf, sizeof(pathbuf), fildes);

    if (_close == NULL) {
        wait_injector_init();
    }
    int ret = _close(fildes);

    logger_printf(LOGGER_HEADER);
    logger_printf("close(\"%s\")", pathbuf);
    logger_printf(" = %d\n", ret);
    return ret;
}

int creat(const char *path, mode_t mode)
{
    char pathbuf[PATH_MAX];

    if (_creat == NULL) {
        wait_injector_init();
    }
    int ret = _creat(path, mode);

    logger_fmt_path(pathbuf, sizeof(pathbuf), path);
    logger_printf(LOGGER_HEADER);
    logger_printf("creat(\"%s\", %03o)", pathbuf, mode);
    logger_printf(" = %d\n", ret);
    return ret;
}

int fclose(FILE *stream)
{
    char pathbuf[PATH_MAX];

    logger_fmt_fp(pathbuf, sizeof(pathbuf), stream);

    if (_fclose == NULL) {
        wait_injector_init();
    }
    int ret = _fclose(stream);

    logger_printf(LOGGER_HEADER);
    logger_printf("fclose(\"%s\")", pathbuf);
    logger_printf(" = %d\n", ret);
    return ret;
}

FILE *fopen(const char *pathname, const char *mode)
{
    char pathbuf[PATH_MAX];

    if (_fopen == NULL) {
        wait_injector_init();
    }
    FILE *ret = _fopen(pathname, mode);

    logger_fmt_path(pathbuf, sizeof(pathbuf), pathname);
    logger_printf(LOGGER_HEADER);
    logger_printf("fopen(\"%s\", \"%s\")", pathbuf, mode);
    logger_printf(" = %p\n", ret);
    return ret;
}

size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    char databuf[256];
    char pathbuf[PATH_MAX];

    if (_fread == NULL) {
        wait_injector_init();
    }
    size_t ret = _fread(ptr, size, nmemb, stream);

    logger_fmt_fp(pathbuf, sizeof(pathbuf), stream);
    logger_fmt_buffer(databuf, sizeof(databuf), ptr, ret);
    logger_printf(LOGGER_HEADER);
    logger_printf("fread(\"%s\", %zu, %zu, \"%s\")", databuf, size, nmemb, pathbuf);
    logger_printf(" = %zu\n", ret);
    return ret;
}

size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    char databuf[256];
    char pathbuf[PATH_MAX];

    if (_fwrite == NULL) {
        wait_injector_init();
    }
    size_t ret = _fwrite(ptr, size, nmemb, stream);

    logger_fmt_fp(pathbuf, sizeof(pathbuf), stream);
    logger_fmt_buffer(databuf, sizeof(databuf), ptr, ret);
    logger_printf(LOGGER_HEADER);
    logger_printf("fwrite(\"%s\", %zu, %zu, \"%s\")", databuf, size, nmemb, pathbuf);
    logger_printf(" = %zu\n", ret);
    return ret;
}

int open(const char *path, int oflag, mode_t mode)
{
    char pathbuf[PATH_MAX];

    if (_open == NULL) {
        wait_injector_init();
    }
    int ret = _open(path, oflag, mode);

    logger_fmt_path(pathbuf, sizeof(pathbuf), path);
    logger_printf(LOGGER_HEADER);
    logger_printf("open(\"%s\", %o, %03o)", pathbuf, oflag, mode);
    logger_printf(" = %d\n", ret);
    return ret;
}

ssize_t read(int fildes, void *buf, size_t nbyte)
{
    char databuf[256];
    char pathbuf[PATH_MAX];

    if (_read == NULL) {
        wait_injector_init();
    }
    ssize_t ret = _read(fildes, buf, nbyte);

    logger_fmt_fd(pathbuf, sizeof(pathbuf), fildes);
    if (ret > 0) {
        logger_fmt_buffer(databuf, sizeof(databuf), buf, (size_t)ret);
    } else {
        databuf[0] = '\0';
    }
    logger_printf(LOGGER_HEADER);
    logger_printf("read(\"%s\", \"%s\", %zu)", pathbuf, databuf, nbyte);
    logger_printf(" = %zd\n", ret);
    return ret;
}

int remove(const char *pathname)
{
    char pathbuf[PATH_MAX];
    logger_fmt_path(pathbuf, sizeof(pathbuf), pathname);

    if (_remove == NULL) {
        wait_injector_init();
    }
    int ret = _remove(pathname);

    logger_printf(LOGGER_HEADER);
    logger_printf("remove(\"%s\")", pathbuf);
    logger_printf(" = %d\n", ret);
    return ret;
}

int rename(const char *old, const char *new)
{
    char oldpathbuf[PATH_MAX];
    char newpathbuf[PATH_MAX];

    logger_fmt_path(oldpathbuf, sizeof(oldpathbuf), old);

    if (_rename == NULL) {
        wait_injector_init();
    }
    int ret = _rename(old, new);

    logger_fmt_path(newpathbuf, sizeof(newpathbuf), new);
    logger_printf(LOGGER_HEADER);
    logger_printf("rename(\"%s\", \"%s\")", oldpathbuf, newpathbuf);
    logger_printf(" = %d\n", ret);
    return ret;
}

FILE *tmpfile()
{
    if (_tmpfile == NULL) {
        wait_injector_init();
    }
    FILE *ret = _tmpfile();

    logger_printf(LOGGER_HEADER);
    logger_printf("tmpfile()");
    logger_printf(" = %p\n", ret);
    return ret;
}

ssize_t write(int fildes, const void *buf, size_t nbyte)
{
    if (fildes == logger_fd()) {
        // Logger print, skip
        if (_write == NULL) {
            wait_injector_init();
        }
        return _write(fildes, buf, nbyte);
    }

    char databuf[256];
    char pathbuf[PATH_MAX];

    if (_write == NULL) {
        wait_injector_init();
    }
    ssize_t ret = _write(fildes, buf, nbyte);

    logger_fmt_fd(pathbuf, sizeof(pathbuf), fildes);
    logger_fmt_buffer(databuf, sizeof(databuf), buf, nbyte);
    logger_printf(LOGGER_HEADER);
    logger_printf("write(\"%s\", \"%s\", %zu)", pathbuf, databuf, nbyte);
    logger_printf(" = %zd\n", ret);
    return ret;
}

