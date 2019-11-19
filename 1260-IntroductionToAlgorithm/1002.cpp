#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

#define LAST(dp, i) (dp[(i - 1) & 1])
#define THIS(dp, i) (dp[i & 1])

struct Assignment {
    unsigned score, time, due;

    Assignment(unsigned s, unsigned t, unsigned d)
        :score(s), time(t), due(d)
    {}

    friend bool operator<(const Assignment &a, const Assignment &b)
    {
        return (a.due == b.due ? a.time < b.time : a.due < b.due);
    }
};

int main()
{
    int n;
    scanf("%d", &n);
    vector<Assignment> a;
    a.reserve(n);
    unsigned max_time = 0;
    for (int i = 0; i < n; i++) {
        unsigned s, t, d;
        scanf("%u%u%u", &s, &d, &t);
        max_time = max(max_time, d);
        a.emplace_back(s, t, d);
    }
    sort(a.begin(), a.end());

    vector<vector<long long>> dp(2, vector<long long>(max_time + 1));

    for (size_t w = 0; w < a.size(); w++) {
        const Assignment &asmt = a[w];
        for (unsigned i = 0; i <= max_time; i++) {
            if (i < asmt.time) {
                THIS(dp, w)[i] = LAST(dp, w)[i];
                continue;
            }
            if (i > asmt.due) {
                THIS(dp, w)[i] = max(THIS(dp, w)[i - 1], LAST(dp, w)[i]);
                continue;
            }
            THIS(dp, w)[i] = max(LAST(dp, w)[i], LAST(dp, w)[i - asmt.time] + asmt.score);
        }
        /*
        for (size_t i = 0; i < THIS(dp, w).size(); i += 100) {
            fprintf(stderr, "%5lld ", THIS(dp, w)[i]);
        }
        fprintf(stderr, "\n");
        */
    }
    
    printf("%lld\n", LAST(dp, a.size())[max_time]);

    return 0;
}
