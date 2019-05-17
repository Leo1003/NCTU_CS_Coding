#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

struct location {
    location(int x, int y)
        : x(x), y(y)
    {}

    bool operator< (const location &other) const 
    {
        return (this->x == other.x ? this->y < other.y : this->x < other.x);
    }

    bool operator<= (const location &other) const
    {
        return (this->x <= other.x && this->y <= other.y);
    }

    int x, y;
};

bool cmp(const location &a, const location &b)
{
    return a <= b;
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        vector<location> locs, stk;
        int n;
        scanf("%d", &n);
        locs.reserve(n);
        stk.reserve(n);
        for (int i = 0; i < n; i++) {
            int x, y;
            scanf("%d%d", &x, &y);
            locs.emplace_back(x, y);
        }
        sort(locs.begin(), locs.end());
        
        for (int i = 0; i < n; i++) {
            if (stk.empty() || stk.back() <= locs[i]) {
                stk.emplace_back(locs[i]);
            } else {
                vector<location>::iterator it = lower_bound(stk.begin(), stk.end(), locs[i], cmp);
                *it = locs[i];
            }
        }
        printf("%zu\n", stk.size());
    }
    return 0;
}
