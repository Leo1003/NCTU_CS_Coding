#include <stdio.h>

int main()
{
    int n, q, axor[100000 + 1];
    scanf("%d%d", &n, &q);

    int last = 0;
    axor[0] = 0;
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        last ^= t;
        axor[i + 1] = last;
    }
    for (int i = 0; i < q; i++) {
        int l, r;
        scanf("%d%d", &l, &r);
        printf("%d\n", axor[l - 1] ^ axor[r]);
    }
    return 0;
}
