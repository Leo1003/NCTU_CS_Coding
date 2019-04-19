#include <stdio.h>

int fastpower(long long a, int b, int m)
{
    long long t = 1;
    if (b == 0) {
        return 1;
    }
    while (b > 1) {
        if (b % 2) {
            t = (t * a) % m;
        }
        a = (a * a) % m;
        b /= 2;
    }
    t = (t * a) % m;
    return t;
}

int main() 
{
    int T;
    scanf("%d", &T);
    for (int i = 0; i < T; i++) {
        int a, p;
        scanf("%d%d", &a, &p);
        int x = fastpower(a, p - 2, p);
        printf("%d\n", x);
    }
    return 0;
}
