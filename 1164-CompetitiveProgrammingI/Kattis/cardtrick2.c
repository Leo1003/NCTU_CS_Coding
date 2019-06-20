#include <stdio.h>

int main()
{
    int T;
    int cards[13];
    scanf("%d", &T);
    while (T--) {
        int n;
        scanf("%d", &n);

        for (int i = 0; i < n; i++) {
            cards[i] = -1;
        }

        int pos = 0;
        for (int i = 1; i <= n; i++) {
            int left = i;
            while (left) {
                pos = (pos + 1) % n;
                if (cards[pos] == -1) {
                    left--;
                }
            }
            cards[pos] = i;
            if (i == n) {
                break;
            }
            left = 1;
            while (left) {
                pos = (pos + 1) % n;
                if (cards[pos] == -1) {
                    left--;
                }
            }
        }

        for (int i = 0; i < n; i++) {
            printf("%d ", cards[i]);
        }
        printf("\n");
    }
    return 0;
}
