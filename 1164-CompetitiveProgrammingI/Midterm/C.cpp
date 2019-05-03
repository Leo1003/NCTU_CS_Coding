#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

bool greedy(const vector<int> &v, int split, long long limit)
{
    long long cur_voc = 0;
    int cur_split = 1;
    size_t i = 0;
    while (i < v.size()) {
        if (cur_split > split) {
            return false;
        }
        if (cur_voc + v[i] > limit) {
            cur_voc = 0;
            cur_split++;
        } else {
            cur_voc += v[i];
            i++;
        }
    }
    return true;
}

long long binary_search_range(const vector<int> &v, long long l, long long r, int split)
{
    if (l == r) {
        if (greedy(v, split, r)) {
            return r;
        } else {
            return r + 1;
        }
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
    int T;
    cin >> T;
    for (int t = 0; t < T; t++) {
        int n, k, single_max = 0;
        long long total = 0;
        cin >> n >> k;
        vector<int> v;
        v.reserve(n);
        for (int i = 0; i < n; i++) {
            int a;
            cin >> a;
            single_max = max(single_max, a);
            total += single_max;
            v.push_back(a);
        }
        
        if (n <= k) {
            cout << single_max << "\n";
            continue;
        }
        
        if (k == 1) {
            cout << total << "\n";
            continue;
        }
        
        cout << binary_search_range(v, single_max, total, k) << "\n";
    }
    return 0;
}

