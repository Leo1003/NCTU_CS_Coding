#include <algorithm>
#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

inline int min3(int a, int b, int c)
{
    return min(min(a, b), c);
}

int LevenshteinDistcance(const string &s1, const string &s2, int add, int modify, int remove)
{
    vector<vector<int>> dp(2, vector<int>(s2.size() + 1, 0));
    for (size_t j = 0; j <= s2.size(); j++) {
        dp[0][j] = j * remove;
    }
    for (size_t i = 1; i <= s1.size(); i++) {
        size_t cur = i % 2, other = (i + 1) % 2;
        dp[cur][0] = i * add;
        for (size_t j = 1; j <= s2.size(); j++) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[cur][j] = min3(dp[other][j - 1], dp[other][j] + add, dp[cur][j - 1] + remove);
            } else {
                dp[cur][j] = min3(dp[other][j - 1] + modify, dp[other][j] + add, dp[cur][j - 1] + remove);
            }
        }
    }
    return dp[(s1.size()) % 2][s2.size()];
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        int n, m, x, y, z;
        scanf("%d%d%d%d%d", &n, &m, &x, &y, &z);
        char correct[1001], kotoha[1001];
        scanf("%1000s", correct);
        scanf("%1000s", kotoha);
        printf("%d\n", LevenshteinDistcance(correct, kotoha, x, z, y));
    }
    return 0;
}
