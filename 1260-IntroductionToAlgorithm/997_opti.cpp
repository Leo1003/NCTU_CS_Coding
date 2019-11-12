#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;

int main()
{
    int n;
    scanf("%d", &n);
    vector<int> A(n);
    int sum = 0;
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
        sum += A[i];
    }

    vector<bool> dp(sum / 2 + 1);
    dp[0] = true;
    for (int i = 0; i <= n; i++) {
        for (int j = sum / 2; j >= 0; j--) {
            if (i > 0 && j - A[i - 1] >= 0) {
                dp[j] = dp[j] || dp[j - A[i - 1]];
            }
        }
    }
    int ans = INT_MAX;
    for (int j = sum / 2; j >= 0; j--) {
        if (dp[j]) {
            ans = abs((sum - j) - j);
            break;
        }
    }
    printf("%d\n", ans);
    return 0;
}
