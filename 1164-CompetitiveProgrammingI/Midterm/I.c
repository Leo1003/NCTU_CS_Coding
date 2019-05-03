#include <stdio.h>

int fastpower(long long a, int n, int m)
{
    long long t = 1;
    while (n > 1) {
        if (n % 2 == 1) {
            t = (t * a) % m;
        }
        n /= 2;
        a = (a * a) % m;
    }
    return (t * a) % m;
}

int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        int k;
        scanf("%d", &k);
        int ans = fastpower(2, k + 1, 998244353);
        ans -= (k + 2);
        if (ans < 0) {
            ans += 998244353;
        }
        printf("%d\n", ans);
    }
    return 0;
}
