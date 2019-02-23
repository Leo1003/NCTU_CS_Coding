#include <stdio.h>
#include <stdlib.h>

int gcd(int a, int b)
{
    a = abs(a);
    b = abs(b);
    int t;
    while (b) {
        t = a;
        a = b;
        b = t % b;
    }
    return a;
}

int main() 
{
    int x, y;
    scanf("%d%d", &x, &y);
    printf("%d\n", gcd(x, y));
    return 0;
}

