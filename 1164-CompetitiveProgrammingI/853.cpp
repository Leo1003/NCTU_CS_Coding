#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else 
# define eprintf(fmt, ...)
#endif

vector<pair<int, int>> merge_same(const vector<int> &v)
{
    vector<pair<int, int>> merged;
    merged.reserve(v.size());
    int p = -1, count = 0;
    for (int weapon: v) {
        if (p != weapon) {
            if (p != -1) {
                merged.push_back(pair<int, int>(p, count));
            }
            p = weapon;
            count = 0;
        }
        count++;
    }
    if (p != -1) {
        merged.push_back(pair<int, int>(p, count));
    }
    
    return merged;
}

inline int get_power(const vector<pair<int, int>> &v, int index)
{
    if (index < 0) {
        return 0;
    }
    return v.at(index).first;
}

inline long long get_count(const vector<pair<int, int>> &v, int index)
{
    if (index < 0) {
        return 1;
    }
    return v.at(index).second;
}

bool cmp(pair<int, int> a, int b)
{
    return a.first < b;
}

int main()
{
    int n, m, q;
    scanf("%d%d%d", &n, &m, &q);
    vector<int> swords, shields;
    swords.reserve(n);
    shields.reserve(m);

    int t;
    for (int i = 0; i < n; i++) {
        scanf("%d", &t);
        swords.push_back(t);
    }
    sort(swords.begin(), swords.end());
    vector<pair<int, int>> swords_comb = merge_same(swords);

    
    for (int i = 0; i < m; i++) {
        scanf("%d", &t);
        shields.push_back(t);
    }
    sort(shields.begin(), shields.end());
    vector<pair<int, int>> shields_comb = merge_same(shields);

    for (int i = 0; i < q; i++) {
        scanf("%d", &t);
        long long ans = 0;
        auto it = lower_bound(swords_comb.begin(), swords_comb.end(), t, cmp);
        int sw_i;
        if (it == swords_comb.end()) {
            sw_i = swords_comb.size() - 1;
        } else {
            sw_i = it - swords_comb.begin();
        }
        int sh_i = -1;

        while (1) {
            eprintf("cur = %d + %d\n", get_power(swords_comb, sw_i), get_power(shields_comb, sh_i));
            long long cur = get_power(swords_comb, sw_i) + get_power(shields_comb, sh_i);
            if (cur > t) {
                if (sw_i == -1) {
                    break;
                }
                sw_i--;
            } else if (cur < t) {
                if (sh_i == (int)shields_comb.size() - 1) {
                    break;
                }
                sh_i++;
            } else {
                ans += get_count(swords_comb, sw_i) * get_count(shields_comb, sh_i);
                if (sw_i == -1) {
                    break;
                }
                sw_i--;
            }
        }

        printf("%lld\n", ans);
    }
    return 0;
}
