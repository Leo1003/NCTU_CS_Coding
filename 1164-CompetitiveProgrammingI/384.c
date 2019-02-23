#include <stdio.h>
#include <limits.h>

#define MAX(a, b) (a > b ? a : b)

int main()
{
    int n, i, sum = 0, max = INT_MIN;
    scanf("%d", &n);
    for (i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        sum = MAX(sum + t, t);
        max = MAX(max, sum);
    }
    printf("%d\n", max);
    return 0;
}
