#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define PREALLOC 1
#define CARRY_THR 1000000000

#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

typedef struct bn {
    unsigned long long *data;
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
    bn->data = (unsigned long long *)malloc(sizeof(unsigned long long) * bn->alloc);
    if (!bn->data) {
        free(bn);
        return NULL;
    }
    memset(bn->data, 0, sizeof(unsigned long long) * bn->alloc);
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
    size_t new_len = digits;
    if (bn->alloc >= new_len) {
        return bn->alloc;
    }
    unsigned long long *tmp = (unsigned long long *)realloc(bn->data, sizeof(unsigned long long) * new_len);
    if (!tmp) {
        return -1;
    }
    bn->data = tmp;
    memset(bn->data+bn->alloc, 0, sizeof(unsigned long long) * (new_len - bn->alloc));
    bn->alloc = new_len;
    return bn->alloc;
}

int bignum_cpy(bignum_t *dest, const bignum_t *src)
{
    if (bignum_alloc(dest, src->alloc) < 0) {
        return -1;
    }
    memcpy(dest->data, src->data, sizeof(unsigned long long) * MIN(dest->alloc, src->alloc));
    dest->top = src->top;
    return 0;
}

int bignum_is_zero(bignum_t *bn)
{
    return (bn->top == 0 && bn->data[0] == 0);
}

int bignum_is_one(bignum_t *bn)
{
    return (bn->top == 0 && bn->data[0] == 1);
}

int bignum_set(bignum_t *bn, unsigned long long i)
{
    if (bignum_alloc(bn, 3) < 0) {
        return -1;
    }
    memset(bn->data, 0, sizeof(unsigned long long) * bn->alloc);
    size_t d = 0;
    do {
        bn->data[d++] = i % CARRY_THR;
        i /= CARRY_THR;
    } while (i);
    
    bn->top = d - 1;
    
    return 0;
}

int bignum_set_str(bignum_t *bn, const char *str)
{
    size_t len = strlen(str);
    if (len == 0) {
        return -1;
    }
    if (bignum_alloc(bn, (len / 9 + 1)) < 0) {
        return -1;
    }
    memset(bn->data, 0, sizeof(unsigned long long) * bn->alloc);
    for (int i = len - 1; i >= 0; i--) {
        int d = len - i - 1;
        if (str[i] < '0' || str[i] > '9') {
            return -1;
        }
        int loc = d / 9;
        unsigned long long digit = str[i] - '0';
        for (int j = 0; j < (d % 9); j++) {
            digit *= 10;
        }
        bn->data[loc] += digit;
        if (bn->data[loc] > 0) {
            bn->top = loc;
        }
    }
    return 0;
}

int bignum_fprint(bignum_t *bn, FILE *fp)
{
    for (int i = bn->top; i >= 0; i--) {
        if (i == bn->top) {
            fprintf(fp, "%llu", bn->data[i]);
        } else {
            fprintf(fp, "%09llu", bn->data[i]);
        }
    }
    return 0;
}

int bignum_print(bignum_t *bn)
{
    return bignum_fprint(bn, stdout);
}

int bignum_add(bignum_t *bn, bignum_t *a)
{
    if (bignum_is_zero(a)) {
        return 0;
    }
    if (bignum_is_zero(bn)) {
        bignum_cpy(bn, a);
        return 0;
    }
    if (bignum_alloc(bn, MAX(bn->top + 1, a->top + 1) + 1) < 0) {
        return -1;
    }
    size_t d = 0;
    for (d = 0; d <= bn->top || d <= a->top; d++) {
        unsigned long long t = 0;
        if (d <= bn->top) {
            t += bn->data[d];
        }
        if (d <= a->top) {
            t += a->data[d];
        }
        bn->data[d] = t;
        /* Determine carry */
        if (bn->data[d] >= CARRY_THR) {
            if (bn->top < d + 1) {
                bn->top = MAX(bn->top, d + 1);
                bn->data[d + 1] = 0;
            }
            bn->data[d + 1] += bn->data[d] / CARRY_THR;
            bn->data[d] %= CARRY_THR;
        }
        if (bn->data[d] > 0) {
            bn->top = MAX(bn->top, d);
        }
    }
    return 0;
}

int bignum_add_at(bignum_t *bn, unsigned long long i, size_t offset)
{
    if (i == 0) {
        return 0;
    }
    if (bignum_alloc(bn, offset + 3 + 1) < 0) {
        return -1;
    }
    size_t d;
    for (d = offset; i || bn->data[d] >= CARRY_THR; d++) {
        if (i) {
            bn->data[d] += i % CARRY_THR;
            i /= CARRY_THR;
        }
        /* Determine carry */
        if (bn->data[d] >= CARRY_THR) {
            if (bn->top < d + 1) {
                bn->top = d + 1;
                bn->data[d + 1] = 0;
            }
            bn->data[d + 1] += bn->data[d] / CARRY_THR;
            bn->data[d] %= CARRY_THR;
        }
        if (bn->data[d] > 0) {
            bn->top = MAX(bn->top, d);
        }
    }
        
	return 0;
}

int bignum_mul(bignum_t *a, bignum_t *b)
{
    /* Fast determine multiple 0 */
    if (bignum_is_zero(a) || bignum_is_zero(b)) {
        bignum_set(a, 0);
        return 0;
    }

    if (bignum_is_one(a)) {
        bignum_cpy(a, b);
        return 0;
    }
    if (bignum_is_one(b)) {
        return 0;
    }

    bignum_t *tmp = bignum_new();
    if (!tmp) {
        return -1;
    }
    if (bignum_alloc(tmp, (a->top + 1) + (b->top + 1)) < 0) {
        free(tmp);
        return -1;
    }

    for (int i = 0; i <= a->top; i++) {
        for (int j = 0; j <= b->top; j++) {
            unsigned long long t = a->data[i] * b->data[j];
            bignum_add_at(tmp, t, i + j);
        }
    }

    if (bignum_cpy(a, tmp) < 0) {
        return -1;
    }

    bignum_free(tmp);
    return 0;
}

static char *get_digits(char *s, char *buf, size_t len)
{
    char *p = s;
    while (*p >= '0' && *p <= '9') {
        p++;
    }
    size_t cpy_len = MIN(p - s, len - 1);
    memcpy(buf, s, cpy_len);
    buf[cpy_len] = '\0';
    return p;
}

int main()
{
    char *line = NULL;
    size_t line_len = 0;
    while (getline(&line, &line_len, stdin) != EOF) {
        bignum_t *ans = bignum_new();
        if (!ans) {
            abort();
        }
        bignum_t *tmp = bignum_new();
        if (!tmp) {
            abort();
        }

        char buf[300 + 1], *pos = line;
        pos = get_digits(pos, buf, sizeof(buf));
        bignum_set_str(ans, buf);

        while (*pos != '\n' && *pos != '\0') {
            char op = *pos;
            pos++;
            switch (op) {
                case '+':
                case '*':
                    break;
                default:
                    continue;
            }
            pos = get_digits(pos, buf, sizeof(buf));
            bignum_set_str(tmp, buf);

            if (op == '+') {
                bignum_add(ans, tmp);
            } else {
                bignum_mul(ans, tmp);
            }
        }

        bignum_print(ans);
        printf("\n");
        bignum_free(tmp);
        bignum_free(ans);
    }
    free(line);
    return 0;
}
