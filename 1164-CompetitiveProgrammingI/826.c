#include <stdio.h>
#include <string.h>

#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

int chocolate[1001];

int eat(int x, int y)
{
    int eaten = 0;
    for (int j = y; j > 0; j--) {
        if (chocolate[j] >= x) {
            break;
        }
        eaten += x - chocolate[j];
        chocolate[j] = x;
    }
    return eaten;
}

int main()
{
    int n, m, q, left = 0;
    scanf("%d%d%d", &n, &m, &q);
    left = n * m;
    memset(chocolate, 0, sizeof(chocolate));
    for (int i = 0; i < q; i++) {
        int x, y;
        scanf("%d%d", &x, &y);
        int e = eat(x, y);
        left -= e;
        if (e <= 0) {
            printf("QAQ\n");
        } else {
            printf("%d\n", e);
        }
    }
    printf("%d\n", left);
    return 0;
}
