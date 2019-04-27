#include <stdio.h>

int main()
{
    int n, minus_c = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        if (t < 0) {
            minus_c++;
        }
    }
    printf("%d\n", minus_c);
    return 0;
}
