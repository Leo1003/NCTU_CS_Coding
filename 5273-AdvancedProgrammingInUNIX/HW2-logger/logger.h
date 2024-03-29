#ifndef _LOGGER_LOGGER_H
#define _LOGGER_LOGGER_H

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

#define LOGGER_HEADER "[logger] "

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>

extern __thread bool logger_writing;

void logger_init();

int logger_printf(const char *fmt, ...);

int logger_fd();

int logger_fmt_path(char *buf, size_t buflen, const char *path);

int logger_fmt_fd(char *buf, size_t buflen, int fd);

int logger_fmt_fp(char *buf, size_t buflen, FILE *fp);

int logger_fmt_buffer(char *buf, size_t buflen, const unsigned char* data, size_t datalen);

#endif

