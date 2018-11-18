#include<iostream>
#include<vector>
using namespace std;

unsigned long long ans = 0;
vector<int> v;

void merge_sort(int l, int r)
{
    if (l == r)
        return;
    int m = (l + r) / 2;
    merge_sort(l, m);
    merge_sort(m + 1, r);

    vector<int> tmp;
    int pl = l, pr = m + 1;
    while (pl <= m && pr <= r) {
        if (v[pl] <= v[pr]) {
            tmp.push_back(v[pl++]);
        } else {
            tmp.push_back(v[pr++]);
            ans += m - pl + 1;
        }
    }
    while (pl <= m) {
        tmp.push_back(v[pl++]);
    }
    while (pr <= r) {
        tmp.push_back(v[pr++]);
    }
    for (int i = 0; i < tmp.size(); i++) {
        v[l + i] = tmp[i];
    }
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    int T = 0;
    while (++T) {
        int n;
        cin >> n;
        if (n == 0)
            break;
        v.clear();
        ans = 0;
        v.reserve(n);
        for (int i = 0; i < n; i++) {
            int a;
            cin >> a;
            v.push_back(a);
        }
        merge_sort(0, v.size() - 1);
        cout << "Case #" << T << ": " << ans << endl;
    }
}