#define _GNU_SOURCE
#include "logger.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

__thread bool logger_writing = false;
static FILE *logfp = NULL;

void logger_init()
{
    if (logfp != NULL) {
        return;
    }

    const char *parent_logfd = getenv("LOGGER_FD");
    if (parent_logfd) {
        char *endptr;

        errno = 0;
        int fd = strtol(parent_logfd, &endptr, 10);
        if (!(errno != 0 || *endptr != '\0')) {
            logfp = fdopen(fd, "w");
            if (logfp) {
#ifndef NDEBUG
                logger_printf(LOGGER_HEADER);
                logger_printf("Logger inherited! (fd = %d)\n", fd);
#endif
                return;
            }
            // Error fall through
        }
        // Error fall through
    }

    const char *path = getenv("LOGGER_FILE");
    if (path == NULL) {
        // Duplicate the file stream to enable buffering on stderr.
        //logfp = fdopen(fileno(stderr), "w");
        logfp = stderr;
    } else {
        logfp = fopen(path, "w");
        if (logfp) {
            char fdstr[256];
            snprintf(fdstr, sizeof(fdstr), "%d", fileno(logfp));
            setenv("LOGGER_FD", fdstr, 1);
#ifndef NDEBUG
            logger_printf(LOGGER_HEADER);
            logger_printf("Logger initialized! (fd = %d)\n", fileno(logfp));
#endif
        }
    }

    /*
    if (logfp == NULL) {
        return;
    }
    setlinebuf(logfp);
    */
}

int logger_printf(const char *fmt, ...)
{
    if (logfp == NULL) {
        return 0;
    }

    logger_writing = true;

    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(logfp, fmt, ap);
    va_end(ap);

    logger_writing = false;
    return ret;
}

int logger_fd()
{
    if (logfp == NULL) {
        return -1;
    }
    return fileno(logfp);
}

int logger_fmt_path(char *buf, size_t buflen, const char *path)
{
    char local_buf[PATH_MAX];

    if (realpath(path, local_buf) == NULL) {
        strncpy(buf, path, buflen);
        buf[buflen - 1] = '\0';
        return -1;
    }

    strncpy(buf, local_buf, buflen);
    buf[buflen - 1] = '\0';
    return 0;
}

int logger_fmt_fd(char *buf, size_t buflen, int fd)
{
    char proc_path[PATH_MAX];
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);
    
    int ret = logger_fmt_path(buf, buflen, proc_path);
    if (ret < 0) {
        ssize_t len = readlink(proc_path, buf, buflen - 1);
        if (len < 0) {
            return -1;
        }
        buf[len] = '\0';
        return 0;
    }
    return ret;
}

int logger_fmt_fp(char *buf, size_t buflen, FILE *fp)
{
    return logger_fmt_fd(buf, buflen, fileno(fp));
}

int logger_fmt_buffer(char *buf, size_t buflen, const unsigned char* data, size_t datalen)
{
    size_t printlen = MIN(MIN(datalen, 32), buflen - 1);
    if (data == NULL) {
        printlen = 0;
    }
    size_t i;
    for (i = 0; i < printlen; i++) {
        if (isprint(data[i])) {
            buf[i] = data[i];
        } else {
            buf[i] = '.';
        }
    }
    buf[i] = '\0';

    return 0;
}

