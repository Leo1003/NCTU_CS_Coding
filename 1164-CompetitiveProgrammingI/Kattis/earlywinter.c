#include <stdio.h>

int main()
{
    int n, dm;
    scanf("%d%d", &n, &dm);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        if (dm >= t) {
            printf("It hadn't snowed this early in %d years!\n", i);
            return 0;
        }
    }
    printf("It had never snowed this early!\n");
    return 0;
}
