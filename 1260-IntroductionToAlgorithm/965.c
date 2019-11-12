#include <math.h>
#include <stdio.h>

long double cal_ns(long long n)
{
    long double q = floorl(sqrtl(n));
    long double l = floorl(log2l(n));
    return (long double)n * q * l;
}

int main()
{
    int t;
    scanf("%d", &t);
    while (t--) {
        long long k;
        scanf("%lld", &k);
        long long k_ns = k * 1e9;
        long long l = 0, r = (long long)1e12;

        while (l < r - 1) {
            long long m = (l + r) / 2;
            long double nsec = cal_ns(m);
            //fprintf(stderr, "[%lld, %lld)\n", l, r);
            //fprintf(stderr, "nsec = %LF\n", nsec);
            if (nsec > k_ns) {
                r = m;
            } else if (nsec < k_ns) {
                l = m;
            } else {
                l = r = m;
            }
        }

        printf("%lld\n", l);
    }
    return 0;
}

