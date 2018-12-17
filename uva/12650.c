#include <stdio.h>
#include <string.h>

int main()
{
    int n,r;
    unsigned char people[10001];
    while (~scanf("%d%d", &n, &r)) {
        memset(people, 0, sizeof(people));
        unsigned char lost = 0;
        int i;
        for (i = 0; i < r; i++) {
            int t;
            scanf("%d", &t);
            people[t] = 1;
        }
        for (i = 1; i <= n; i++) {
            if (!people[i]) {
                lost = 1;
                printf("%d ", i);
            }
        }
        if (!lost) {
            printf("*");
        }
        printf("\n");
    }
    return 0;
}
