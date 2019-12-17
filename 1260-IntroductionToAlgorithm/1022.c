#include <limits.h>
#include <stdio.h>

typedef struct subarr {
    long long val;
    size_t l, r;
} subarr;

int main()
{
    int n;
    scanf("%d", &n);
    
    subarr max, cur;
    max.val = cur.val = LLONG_MIN;

    for (int i = 1; i <= n; i++) {
        if (cur.val < 0) {
            cur.val = 0;
            cur.l = cur.r = i;
        }

        int t;
        scanf("%d", &t);
        cur.val += t;
        cur.r = i;

        if (max.val < cur.val) {
            max = cur;
        }
    }

    printf("%zu %zu\n", max.l, max.r);
    return 0;
}
