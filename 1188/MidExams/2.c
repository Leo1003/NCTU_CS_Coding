#include <stdio.h>
#define SIZE 10

int N[SIZE][SIZE];

int main()
{
    int n;
    scanf("%d", &n);
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n; i++) {
            scanf("%d", &N[i][j]);
        }
    }
    for (int j = n - 1; j >= 0; j--) {
        for (int i = n - 1; i >= 0; i--) {
            printf("%d\t", N[i][j]);
        }
        printf("\n");
    }
    return 0;
}
