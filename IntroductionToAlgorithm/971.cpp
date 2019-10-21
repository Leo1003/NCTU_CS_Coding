#include <algorithm>
#include <iostream>
#include <limits>
#include <stdio.h>
#include <string>
#include <string.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

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
#ifdef DEBUG
    for (size_t j = 0; j <= s2.size(); j++) {
        debugf("%d ", dp[0][j]);
    }
    debugf("\n");
#endif
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

#ifdef DEBUG
        for (size_t j = 0; j <= s2.size(); j++) {
            debugf("%d ", dp[cur][j]);
        }
        debugf("\n");
#endif
    }
    return dp[(s1.size()) % 2][s2.size()];
}

int main()
{
    int T;
    cin >> T;
    while (T--) {
        int a, b, r;
        string from, into;

        cin >> a >> b >> r;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        getline(cin, from);
        getline(cin, into);

        cout << LevenshteinDistcance(into, from, a, r, b) << endl;
    }
    
    return 0;
}
