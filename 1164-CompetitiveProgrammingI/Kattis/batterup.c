#include <stdio.h>

int main()
{
    int n, count = 0;
    long long total = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        if (t == -1) {
            continue;
        }
        count++;
        total += t;
    }
    if (count == 0) {
        printf("%lf\n", 0.0);
    } else {
        printf("%lf\n", total / (double)count);
    }
    return 0;
}
