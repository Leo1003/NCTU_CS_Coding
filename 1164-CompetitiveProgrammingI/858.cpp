#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define eprintf(fmt, ...)
#endif

inline bool greedy(const vector<long long> &v, int split_limit, long long value_limit)
{
    long long cur_max = 0;
    size_t cur_size = 0;
    int cur_split = 1;
    size_t i = 0;
    while (i < v.size()) {
        if (cur_split > split_limit) {
            return false;
        }
        if ((max(cur_max, v[i]) * (long long)(cur_size + 1)) > value_limit) {
            cur_max = 0;
            cur_size = 0;
            cur_split++;
        } else {
            cur_max = max(cur_max, v[i]);
            cur_size++;
            i++;
        }
    }
    return true;
}

long long binary_search_range(const vector<long long> &v, long long l, long long r, int split)
{
    eprintf("[ %lld, %lld]\n", l, r);
    if (l == r) {
        return r;
    }
    long long m = (l + r) / 2;
    bool cmp = greedy(v, split, m);
    if (cmp) {
        return binary_search_range(v, l, m, split);
    } else {
        return binary_search_range(v, m + 1, r, split);
    }
}

int main()
{
    long long value_min = 0, value_max = 0;
    int n, k;
    scanf("%d%d", &n, &k);
    vector<long long> a;
    a.reserve(n);
    for (int i = 0; i < n; i++) {
        long long t;
        scanf("%lld", &t);
        value_min = max(value_min, t);
        a.push_back(t);
    }
    value_max = value_min * a.size();

    long long ans;
    if (n <= k) {
        ans = value_min;
    } else if (k == 1) {
        ans = value_max;
    } else {
        ans = binary_search_range(a, value_min, value_max, k);
    }

    printf("%lld\n", ans);
    return 0;
}
