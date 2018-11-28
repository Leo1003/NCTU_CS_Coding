#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int map[20][20];
int mx, my;
int m, n;

int search(int x, int y, int dx, int dy, int color, int num)
{
    if (num >= 5) {
        return 1;
    }
    if (x < 0 || x >= m || y < 0 || y >= n) {
        return 0;
    }
    if (map[x][y] != color) {
        return 0;
    }
    int ret = search(x + dx, y + dy, dx, dy, color, num + 1);
    if (ret && num == 2) {
        mx = x;
        my = y;
    }
    return ret;
}

int main()
{
    scanf("%d%d", &m, &n);
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            scanf("%d", &map[i][j]);
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j< n; j++) {
            if (map[i][j]) {
                if (search(i, j, 0, 1, map[i][j], 0)) {
                    goto out;
                }
                if (search(i, j, 1, 0, map[i][j], 0)) {
                    goto out;
                }
                if (search(i, j, 1, 1, map[i][j], 0)) {
                    goto out;
                }
                if (search(i, j, -1, 1, map[i][j], 0)) {
                    goto out;
                }
            }
        }
    }
    return 0;
out:
    printf("%d\n%d", mx + 1, my + 1);
    return 0;
}
