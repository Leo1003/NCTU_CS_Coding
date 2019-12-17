#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

long long cal_blocks(const char *s, size_t len, size_t *loc, long long block_size)
{
    long long pixels = 0;
    switch (s[(*loc)++]) {
        case '2':
            for (size_t i = 0; i < 4 && *loc < len; i++) {
                pixels += cal_blocks(s, len, loc, block_size / 4);
            }
            break;
        case '1':
            pixels += block_size;
            break;
        case '0':
            break;
        default:
            break;
    }

    return pixels;
}

int main()
{
    int k;
    scanf("%d", &k);
    while (getchar() != '\n');

    char *str = NULL;
    size_t str_l = 0;
    getline(&str, &str_l, stdin);

    size_t l = 0;
    printf("%lld\n", cal_blocks(str, strlen(str), &l, (long long)k * (long long)k));

    free(str);
    return 0;
}
