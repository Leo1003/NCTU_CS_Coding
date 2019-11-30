#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

class DJS {
public:
    DJS(size_t n)
        : g(n + 1), cnt(n + 1)
    {
        for (size_t i = 1; i <= n; i++) {
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

    int cfind(int c) const
    {
        while (c != g[c]) {
            c = g[c];
        }
        return c;
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

#ifdef DEBUG
    void debug(const string &title) const
    {
        fprintf(stderr, "------ %s ------\n", title.c_str());
        for (size_t i = 1; i < g.size(); i++) {
            fprintf(stderr, "%d ", g[i]);
        }
        fprintf(stderr, "\n");
    }
#endif
private:
    vector<int> g, cnt;
};

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    DJS djs(n);
    
    for (int j = 0; j < m; j++) {
        int a, b;
        scanf("%d%d", &a, &b);
        djs.group_union(a, b);
    }
#ifdef DEBUG
    djs.debug("Bridged");
#endif

    for (int i = 1; i <= n; i++) {
        printf("%d " , djs.group_count(i) - 1);
    }
    printf("\n");
    return 0;
}
