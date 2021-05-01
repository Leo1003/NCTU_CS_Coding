#define _GNU_SOURCE
#include "logger.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static FILE *logfp = NULL;

void logger_init()
{
    if (logfp != NULL) {
        return;
    }

    const char *path = getenv("LOGGER_FILE");
    if (path == NULL) {
        // Duplicate the file stream to enable buffering on stderr.
        logfp = fdopen(fileno(stderr), "w");
    } else {
        logfp = fopen(path, "w");
    }

    if (logfp == NULL) {
        return;
    }
    setlinebuf(logfp);
}

int logger_printf(const char *fmt, ...)
{
    if (logfp == NULL) {
        logger_init();
        if (logfp == NULL) {
            return 0;
        }
    }

    va_list ap;
    va_start(ap, fmt);
    int ret = vfprintf(logfp, fmt, ap);
    va_end(ap);
    return ret;
}

int logger_fmt_path(char *buf, size_t buflen, const char *path)
{
    char local_buf[PATH_MAX];

    if (realpath(path, local_buf) == NULL) {
        strncpy(buf, path, buflen);
        buf[buflen - 1] = '\0';
        return 0;
    }

    strncpy(buf, local_buf, buflen);
    buf[buflen - 1] = '\0';
    return 0;
}

int logger_fmt_fd(char *buf, size_t buflen, int fd)
{
    char proc_path[PATH_MAX];
    snprintf(proc_path, sizeof(proc_path), "/proc/self/fd/%d", fd);
    
    return logger_fmt_path(buf, buflen, proc_path);
}

int logger_fmt_fp(char *buf, size_t buflen, FILE *fp)
{
    return logger_fmt_fd(buf, buflen, fileno(fp));
}

int logger_fmt_buffer(char *buf, size_t buflen, const unsigned char* data, size_t datalen)
{
    size_t printlen = MIN(MIN(datalen, 32), buflen - 1);
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

