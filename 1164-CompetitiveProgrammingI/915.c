#include <stdio.h>
#include <stdbool.h>

bool nim(int heap[4])
{
    int nim_sum = 0;
    for (int i = 0; i < 4; i++) {
        nim_sum ^= heap[i];
    }
    return nim_sum > 0;
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        int r, c, x, y;
        scanf("%d%d%d%d", &r, &c, &x, &y);
        int h[4];
        h[0] = x - 1;
        h[1] = y - 1;
        h[2] = r - x;
        h[3] = c - y;
        if (nim(h)) {
            printf("Taberu\n");
        } else {
            printf("Tabehen\n");
        }
    }
    return 0;
}
