#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PREALLOC 20

#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

typedef struct bn {
    unsigned char *data;
    size_t top, alloc;
} bignum_t;

bignum_t *bignum_new()
{
    bignum_t *bn = (bignum_t *)malloc(sizeof(bignum_t));
    if (!bn) {
        return NULL;
    }
    bn->top = 0;
    bn->alloc = PREALLOC;
    bn->data = (unsigned char *)malloc(sizeof(unsigned char) * bn->alloc);
    if (!bn->data) {
        free(bn);
        return NULL;
    }
    memset(bn->data, 0, sizeof(unsigned char) * bn->alloc);
    return bn;
}

void bignum_free(bignum_t *bn)
{
    free(bn->data);
    bn->data = NULL;
    free(bn);
}

int bignum_alloc(bignum_t *bn, int digits)
{
    if (bn->alloc >= digits) {
        return bn->alloc;
    }
    unsigned char *tmp = (unsigned char *)realloc(bn->data, sizeof(unsigned char) * digits);
    if (!tmp) {
        return -1;
    }
    bn->data = tmp;
    bn->alloc = digits;
    return bn->alloc;
}

int bignum_cpy(bignum_t *dest, const bignum_t *src)
{
    if (bignum_alloc(dest, src->alloc) < 0) {
        return -1;
    }
    memcpy(dest->data, src->data, sizeof(unsigned char) * dest->alloc);
    dest->top = src->top;
    return 0;
}

int bignum_set(bignum_t *bn, unsigned long long i)
{
    if (bignum_alloc(bn, 20) < 0) {
        return -1;
    }
    size_t d = 0;
    while (i) {
        bn->data[d++] = i % 10;
        i /= 10;
    }
    if (d > 0) {
        bn->top = d - 1;
    } else {
        bn->top = 0;
    }
    return 0;
}

int bignum_set_str(bignum_t *bn, const char *str)
{
    size_t len = strlen(str);
    if (len == 0) {
        return -1;
    }
    if (bignum_alloc(bn, len) < 0) {
        return -1;
    }
    for (int i = len - 1; i >= 0; i--) {
        if (str[i] < '0' || str[i] > '9') {
            return -1;
        }
        bn->data[len - i - 1] = str[i] - '0';
        if (bn->data[len - i - 1] > 0) {
            bn->top = len - i - 1;
        }
    }
    return 0;
}

int bignum_fprint(bignum_t *bn, FILE *fp)
{
    for (int i = bn->top; i >= 0; i--) {
        fputc('0' + bn->data[i], fp);
    }
    return 0;
}

int bignum_print(bignum_t *bn)
{
    return bignum_fprint(bn, stdout);
}

int bignum_add(bignum_t *bn, bignum_t *a)
{
    if (bignum_alloc(bn, MAX(bn->top + 1, a->top + 1) + 1) < 0) {
        return -1;
    }
    size_t d = 0;
    for (d = 0; d <= bn->top || d <= a->top; d++) {
        int t = 0;
        if (d <= bn->top) {
            t += bn->data[d];
        }
        if (d <= a->top) {
            t += a->data[d];
        }
        bn->data[d] = t;
        /* Determine carry */
        if (bn->data[d] >= 10) {
            if (bn->top < d + 1) {
                bn->top = d + 1;
                bn->data[d + 1] = 0;
            }
            bn->data[d + 1] += bn->data[d] / 10;
        }
        bn->data[d] %= 10;
        bn->top = MAX(bn->top, d);
    }
    return 0;
}

int main()
{
    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, stdin) != EOF) {
        size_t l = strlen(line);
        bignum_t *ans = bignum_new();
        if (!ans) {
            return 1;
        }
        char *start = NULL;
        for (int i = 0; start || i < l; i++) {
            if (i >= l || line[i] < '0' || line[i] > '9') {
                line[i] = '\0';
                if (start) {
                    bignum_t *tmp = bignum_new();
                    bignum_set_str(tmp, start);
                    bignum_add(ans, tmp);
                    bignum_free(tmp);
                    start = NULL;
                }
            } else {
                if (start == NULL) {
                    start = line + i;
                }
            }
        }

        bignum_print(ans);
        printf("\n");
        bignum_free(ans);
    }
    free(line);
    return 0;
}
