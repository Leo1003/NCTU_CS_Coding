#include <stdio.h>

int main()
{
    int t;
    scanf("%d", &t);
    while (t--) {
        int n;
        scanf("%d", &n);
        int sum = 1, d, max = n;
        for (d = 2; d < max; d++) {
            if (n % d == 0) {
                sum += d;
                if (d != n / d) {
                    sum += n / d;
                }
                max = n / d;
            }
        }
        if (sum < n) {
            printf("deficient\n");
        } else if (sum == n) {
            printf("perfect\n");
        } else {
            printf("abundant\n");
        }
    }
    return 0;
}
