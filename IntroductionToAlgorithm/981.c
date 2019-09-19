#define _POSIX_C_SOURCE 201909L
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
# define getcx getchar
#else 
# define getcx getchar_unlocked
#endif

struct freqpair {
    char letter;
    int times;
};

int cmp(const void *a, const void *b) {
    const struct freqpair *fa = (const struct freqpair *)a, *fb = (const struct freqpair *)b;
    if (fa->times == fb->times) {
        return fa->letter - fb->letter;
    }
    return fb->times - fa->times;
}

int main()
{
    int c;
    struct freqpair bucket[26];
    for (int i = 0; i < 26; i++) {
        bucket[i].letter = 'A' + i;
        bucket[i].times = 0;
    }
    while ((c = getcx()) != '\n' && c != EOF) {
        if (c >= 'A' && c <= 'Z') {
            bucket[c - 'A'].times++;
        }
    }
    qsort(bucket, 26, sizeof(struct freqpair), cmp);
    for (int i = 0; i < 26; i++) {
        if (bucket[i].times > 0) {
            printf("%d %d\n", bucket[i].letter, bucket[i].times);
        }
    }
    return 0;
}
