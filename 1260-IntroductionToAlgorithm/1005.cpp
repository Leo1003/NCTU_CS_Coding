#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

struct Assignment {
    int score, time;

    Assignment(int s, int t)
        :score(s), time(t)
    {}
};

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    vector<Assignment> a;
    a.reserve(n);
    for (int i = 0; i < n; i++) {
        int s, t;
        scanf("%d%d", &s, &t);
        a.emplace_back(s, t);
    }

    vector<int> dp(m + 1);

    for (const Assignment &asmt: a) {
        for (int i = m; i >= asmt.time; i--) {
            dp[i] = max(dp[i], dp[i - asmt.time] + asmt.score);
        }
    }
    
    printf("%d\n", dp[m]);

    return 0;
}
