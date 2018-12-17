#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DIGITS 10000

typedef struct bu {
    unsigned short data[DIGITS];
    int top;
} bigint_t;

void bigint_init(bigint_t *bi)
{
    memset(bi->data, 0, sizeof(unsigned short) * DIGITS);
    bi->top = 0;
}

void bigint_set(bigint_t *bi, unsigned long long i)
{
    bigint_init(bi);
    size_t d = 0;
    while (i) {
        bi->data[d++] = i % 10;
        i /= 10;
    }
    if (d) {
        bi->top = d - 1;
    } else {
        bi->top = 0;
    }
}

void bigint_add_offset(bigint_t *bi, unsigned long long a, size_t offset)
{
    size_t d = offset;
    while (a || bi->data[d]) {
        if (a) {
            bi->data[d] += a % 10;
            a /= 10;
        }
        bi->data[d + 1] += bi->data[d] / 10;
        bi->data[d] %= 10;
        d++;
    }
    if (d) {
        bi->top = d - 1;
    } else {
        bi->top = 0;
    }
}

void bigint_mul(bigint_t *bi, unsigned int a)
{
    bigint_t tmp;
    bigint_init(&tmp);
    
    size_t i;
    for (i = 0; i <= bi->top; i++) {
        unsigned long long v = a * bi->data[i];
        bigint_add_offset(&tmp, v, i);
    }
    memcpy(bi->data, tmp.data, sizeof(unsigned short) * DIGITS);
    bi->top = tmp.top;
}

int main() {
    int n;
    while (scanf("%d", &n) != EOF) {
        int i;
        int ans = 0;
        bigint_t bn;
        bigint_set(&bn, 1);
        for (i = 1; i <= n; i++) {
            bigint_mul(&bn, i);
        }
        /*
        fprintf(stderr, "bn = ");
        for (i = bn.top; i >= 0; i--) {
            fprintf(stderr, "%d", bn.data[i]);
        }
        fprintf(stderr, "\n");
        */
        for (i = 0; i <= bn.top; i++) {
            ans += bn.data[i];
        }

        printf("%d\n", ans);
    }
    return 0;
}
