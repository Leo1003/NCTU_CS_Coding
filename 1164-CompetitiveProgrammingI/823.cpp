#include <algorithm>
#include <climits>
#include <cstdio>
#include <vector>
using namespace std;

int add(vector<int> &v, int i)
{       
    while (!v.empty() && v.back() <= i) {
        v.pop_back();
    }
    v.push_back(i);
    return v.size();
}

int main()
{
    int testcase;
    scanf("%d", &testcase);
    while (testcase--) {
        unsigned n;
        int ans = 0;
        scanf("%u",&n);
        vector<int> a(n);
        for (unsigned i = 0; i < n; i++) {
            scanf("%d", &a[i]);
        }

        vector<int> v;
        v.reserve(a.size());
        for (int i = a.size() - 1; i >= 0; i--) {
            ans = max(ans, add(v, a[i]));
        }
        printf("%d\n", ans);
    }
    return 0;
}

