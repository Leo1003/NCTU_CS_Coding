#include <stdio.h>

int main()
{
    int n;
    scanf("%d", &n);
    int ans = n - 2;
    if (ans >= 1) {
        printf("%d\n", ans);
    } else {
        printf("1\n");
    }
    return 0;
}
