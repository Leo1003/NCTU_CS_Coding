#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;

class seq_num {
public:
    int num;
    unsigned seq;
    seq_num(int num, unsigned seq): num(num), seq(seq) 
    {}
};

bool cmp_seq(seq_num a, seq_num b)
{
    return (a.num < b.num);
}

int main()
{
    vector<seq_num> vec;
    int n, q;
    cin >> n >> q;
    vec.reserve(n);
    for (unsigned i = 0; i < n; i++) {
        int t;
        cin >> t;
        seq_num d(t, i + 1);
        vec.push_back(d);
    }
    sort(vec.begin(), vec.end(), cmp_seq);

    for (int i = 0; i < q; i++) {
        int l, r, k, cnt = 0, ans;
        cin >> l >> r >> k;
        for (int j = 0; k != cnt && j < n; j++) {
            if (vec[j].seq >= l && vec[j].seq <= r) {
                cnt++;
                ans = vec[j].num;
            }
        }
        cout << ans << endl;
    }
    return 0;
}
