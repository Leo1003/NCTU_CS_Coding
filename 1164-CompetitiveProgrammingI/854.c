#include <stdio.h>

int exgcd(int a, int b, int *x, int *y)
{
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    } else {
        int r = exgcd(b, a % b, x, y);
        int t = *x;
        *x = *y;
        *y = t - a / b * *y;
        return r;
    }
}

int modinv(int a, int p)
{
    int x, y, r;
    r = exgcd(a, p, &x, &y);
    if (r != 1) {
        return 0;
    } else {
        if (x < 0) {
            x += p;
        }
        return x;
    }
}

int main()
{
    int T;
    scanf("%d", &T);
    for (int i = 0; i < T; i++) {
        int a, p;
        scanf("%d%d", &a, &p);
        int x = modinv(a, p);
        if (x > 0) {
            printf("%d\n", x);
        }
    }
    return 0;
}
