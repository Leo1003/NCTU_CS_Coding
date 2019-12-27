#include <stdio.h>
#include <vector>
using namespace std;

class DJS {
public:
    DJS(size_t n)
        : g(n), cnt(n)
    {
        for (size_t i = 0; i < n; i++) {
            g[i] = i;
            cnt[i] = 1;
        }
    }

    int find(int c)
    {
        if (g[c] != c) {
            g[c] = find(g[c]);
        }
        return g[c];
    }

    bool eq(int a, int b)
    {
        return find(a) == find(b);
    }

    int group_count(int c)
    {
        return cnt[find(c)];
    }

    void group_union(int a, int b)
    {
        int leader_a = find(a);
        int leader_b = find(b);
        if (leader_a == leader_b) {
            return;
        }
        g[leader_b] = leader_a;
        cnt[leader_a] += cnt[leader_b];
        cnt[leader_b] = 0;
    }

private:
    vector<int> g, cnt;
};

