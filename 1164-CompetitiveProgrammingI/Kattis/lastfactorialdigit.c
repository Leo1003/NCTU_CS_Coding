#include <stdio.h>

int main()
{
    int table[11], t = 1;
    table[0] = 1;
    for (int i = 1; i <= 10; i++) {
        t = (t * i) % 10;
        table[i] = t;
    }
    int T;
    scanf("%d", &T);
    while (T--) {
        int n;
        scanf("%d", &n);
        printf("%d\n", table[n]);
    }
    return 0;
}
