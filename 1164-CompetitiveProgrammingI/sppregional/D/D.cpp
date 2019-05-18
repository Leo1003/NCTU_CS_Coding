#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

bool bin_sear(vector<int> &c, int ans)
{
    auto it = upper_bound(c.begin(), c.end(), ans);
    return (distance(it, c.end()) - 1 < ans);
}

int main()
{
    int n;
    scanf("%d", &n);
    vector<int> c;
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        c.push_back(t);
    }
    sort(c.begin(), c.end());
    int l = 0, r = n;
    while (l < r) {
        int mid = (l + r) / 2;
        //fprintf(stderr, "[%d, %d]\n", l, r);
        if (bin_sear(c, mid)) {
            r = mid;
        } else {
            l = mid + 1;
        }
    }
    printf("%d\n", l);
    return 0;
}
