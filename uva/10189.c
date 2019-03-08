#include <stdio.h>

int n, m, table[100][100];

int safe_plus(int x, int y)
{
    if (x < 0 || x >= n || y < 0 || y >= m) {
        return 0;
    }
    table[x][y]++;
    return 1;
}

void mark(int x, int y)
{
    safe_plus(x - 1, y - 1);
    safe_plus(x - 1, y);
    safe_plus(x - 1, y + 1);
    safe_plus(x, y - 1);
    safe_plus(x, y + 1);
    safe_plus(x + 1, y - 1);
    safe_plus(x + 1, y);
    safe_plus(x + 1, y + 1);
}

int main() 
{
    int cnt = 0;
    while (++cnt) {
        scanf("%d%d", &n, &m);
        if (n == 0 && m == 0) {
            break;
        }
        scanf("\n");
        int i, j;
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                char c;
                scanf("%c", &c);
                if (c == '*') {
                    table[i][j] = 9;
                } else {
                    table[i][j] = 0;
                }
            }
            scanf("\n");
        }
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                if (table[i][j] >= 9) {
                    mark(i, j);
                }
            }
        }
        if (cnt > 1) {
            printf("\n");
        }
        printf("Field #%d:\n", cnt);
        for (i = 0; i < n; i++) {
            for (j = 0; j < m; j++) {
                if (table[i][j] >= 9) {
                    printf("*");
                } else {
                    printf("%d", table[i][j]);
                }
            }
            printf("\n");
        }
    }
    return 0;
}
