#include <stdio.h>
#define CURR(x) (x % 2)
#define LAST(x) ((x - 1) % 2)
#define MAX(_a, _b) (_a > _b ? _a : _b)
#define MAX3(_a, _b, _c) MAX(MAX(_a, _b), _c)

struct State {
    // HT: Take Head & Tail
    // HN: Take Head
    // NT: Take Tail
    // NN: Not take Head & Tail
    long long HT, HN, NT, NN;
};

int main()
{
    int n;
    scanf("%d", &n);
    
    struct State dp[2];
    for (int i = 0; i < n; i++) {
        int a;
        scanf("%d", &a);
        if (i == 0) {
            dp[CURR(i)].HT = a;
            dp[CURR(i)].HN = 0;
            dp[CURR(i)].NT = 0;
            dp[CURR(i)].NN = 0;
        } else if (i == 1) {
            dp[CURR(i)].HT = dp[LAST(i)].HT;
            dp[CURR(i)].HN = dp[LAST(i)].HT;
            dp[CURR(i)].NT = a;
            dp[CURR(i)].NN = 0;
        } else {
            dp[CURR(i)].HT = dp[LAST(i)].HN + a;
            dp[CURR(i)].HN = MAX(dp[LAST(i)].HT, dp[LAST(i)].HN);
            dp[CURR(i)].NT = dp[LAST(i)].NN + a;
            dp[CURR(i)].NN = MAX(dp[LAST(i)].NT, dp[LAST(i)].NN);
        }
    }

    if (n == 1) {
        printf("%lld\n", dp[0].HT);
    } else {
        printf("%lld\n", MAX3(dp[LAST(n)].HN, dp[LAST(n)].NT, dp[LAST(n)].NN));
    }
    return 0;
}
