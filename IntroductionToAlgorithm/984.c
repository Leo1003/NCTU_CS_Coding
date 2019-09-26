#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#define getcx getchar
#else
#define getcx getchar_unlocked
#endif

#define MODULE 524287

long long ans = 0;
int data[10000000], tmp[10000001];

inline long long inp()
{
    long long n = 0;
    int ch = getcx();
    int sign = 1;
    while (ch < '0' || ch > '9') {
        if (ch == '-') sign = -1;
        ch = getcx();
    }
    while (ch >= '0' && ch <= '9')
        n = (n << 3) + (n << 1) + ch - '0', ch = getcx();
    n = n * sign;
    return n;
}

void mergesort(size_t l, size_t r)
{
    if (l == r) {
        return;
    }
    size_t m = (l + r) / 2;
    mergesort(l, m);
    mergesort(m + 1, r);
    size_t p1 = l, p2 = m + 1, idx = l;
    while (p1 <= m && p2 <= r) {
        if (data[p1] > data[p2]) {
            tmp[idx++] = data[p2++];
            ans += (m - p1 + 1);
            ans %= MODULE;
        } else {
            tmp[idx++] = data[p1++];
        }
    }
    while (p1 <= m) 
        tmp[idx++] = data[p1++];
    while (p2 <= r) 
        tmp[idx++] = data[p2++];
    memcpy(data + l, tmp + l, sizeof(int) * (r - l + 1));
}

int main()
{
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        data[i] = inp();
    }
    mergesort(0, n - 1);
    printf("%lld\n", ans);
    return 0;
}

