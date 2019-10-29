#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;
#define CURR(x, i) (x[i % 2])
#define LAST(x, i) (x[((i - 1) % 2)])

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

    vector<vector<bool>> dp(2, vector<bool>((sum / 2) + 1));
    for (int i = 0; i <= n; i++) {
        CURR(dp, i)[0] = true;
        for (int j = 1; j <= sum / 2; j++) {
            if (i == 0) {
                CURR(dp, i)[j] = false;
            } else {
                // Take
                if (j - A[i - 1] >= 0) {
                    CURR(dp, i)[j] = CURR(dp, i)[j] || LAST(dp, i)[j - A[i - 1]];
                }
                // Not to Take
                CURR(dp, i)[j] = CURR(dp, i)[j] || LAST(dp, i)[j];
            }
        }
    }
    int ans = INT_MAX;
    for (int j = sum / 2; j >= 0; j--) {
        if (CURR(dp, n)[j]) {
            ans = abs((sum - j) - j);
            break;
        }
    }
    printf("%d\n", ans);
    return 0;
}
