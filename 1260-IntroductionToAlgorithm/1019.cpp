#include <algorithm>
#include <iostream>
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

    int group_size(int c)
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
        cerr << "------ " << title << " ------" << endl;
        for (size_t i = 0; i < g.size(); i++) {
            cerr << g[i] << " ";
        }
        cerr << endl;
    }
#endif
private:
    vector<int> g, cnt;
};

struct Edge {
    int a, b, cost;

    friend istream& operator>>(istream &s, Edge &self)
    {
        s >> self.a >> self.b >> self.cost;
        return s;
    }

    bool operator<(const Edge &other) const
    {
        return this->cost < other.cost;
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m, w;
    cin >> n >> m >> w;

    DJS cities(n);
    vector<Edge> edges;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        Edge e;
        cin >> e;
        if (e.cost < w) {
            edges.emplace_back(e);
        }
    }

    sort(edges.begin(), edges.end());

    long long total_cost = 0;
    int left_cities = n;
    for (Edge &e: edges) {
        if (!cities.eq(e.a, e.b)) {
            total_cost += e.cost;
            cities.group_union(e.a, e.b);
            left_cities--;
        }
        if (left_cities == 1) {
            break;
        }
    }

    total_cost += (left_cities - 1) * w;
    cout << total_cost << endl;
    return 0;
}

