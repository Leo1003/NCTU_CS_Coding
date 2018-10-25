#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PRIME_TABLE 300000
#define PRIME_COUNT 25000

bool * init_primes(size_t len)
{
    if (len < 2) {
        return NULL;
    }
    bool *t = malloc(sizeof(bool) * len);
    if (!t) {
        return NULL;
    }
    memset(t, 0, sizeof(bool) * len);
    t[0] = true;
    t[1] = true;
    for (int i = 2; i < sqrt(len); i++) {
        if (t[i]) {
            continue;
        }
        for (int j = i * i; j < len; j += i) {
            t[j] = true;
        }
    }
    return t;
}

int main()
{
    bool *t = init_primes(PRIME_TABLE + 1);
    if (!t) {
        fprintf(stderr, "alloc error!\n");
        abort();
    }
    int *list = malloc(sizeof(int) * PRIME_COUNT);
    if (!list) {
        fprintf(stderr, "alloc error\n");
        free(t);
        abort();
    }
    memset(list, 0, sizeof(int) * PRIME_COUNT);
    fprintf(stderr, "Initialization completed!\n");

    for (int i = 0, n = 0; i < PRIME_TABLE + 1 && n < PRIME_COUNT; i++) {
        if (!t[i]) {
            list[n++] = i;
        }
    }
    int num;
    while (scanf("%d", &num) != EOF) {
        if (num >= PRIME_COUNT + 1 || num <= 0 || list[num - 1] == 0) {
            printf("Out of bound!!\n");
            continue;
        }
        printf("%d\n", list[num - 1]);
    }
    free(list);
    free(t);
}
