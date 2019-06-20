#include <stdio.h>

int main()
{
    int h, w, n, bricks[10000];
    scanf("%d%d%d", &h, &w, &n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &bricks[i]);
    }

    int b = 0, completed = 1;
    for (int i = 0; i < h; i++) {
        int sum = 0;
        while (sum < w) {
            if (b >= n) {
                break;
            }
            sum += bricks[b++];
        }
        if (sum != w) {
            completed = 0;
            break;
        }
    }
    if (completed) {
        printf("YES\n");
    } else {
        printf("NO\n");
    }

    return 0;
}
