#include <stdio.h>
#define SIZE 100000

long long dp[2][SIZE + 1];
int table = 0;

int build(int n)
{
    for (int i = table + 1; i <= n; i++) {
        dp[0][i] = dp[1][i - 2] + dp[1][i - 4];
        dp[1][i] = dp[0][i - 1] + dp[0][i - 3];
        table++;
    }
    return table;
}

int main()
{
    int n;
    table = 4;
    dp[0][1] = 0;
    dp[1][1] = 1;
    dp[0][2] = 0;
    dp[1][2] = 0;
    dp[0][3] = 1;
    dp[1][3] = 1;
    dp[0][4] = 0;
    dp[1][4] = 1;
    while(~scanf("%d", &n)) {
        if (n > table) {
            build(n);
        }
        printf("%lld\n", dp[0][n] + dp[1][n]);
    }
    return 0;
}

