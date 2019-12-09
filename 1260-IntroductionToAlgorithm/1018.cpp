#include <stdio.h>
#include <vector>
using namespace std;

struct Edge {
    int to, length;

    Edge()
    {
    }

    Edge(int t, int l)
        :to(t), length(l)
    {
    }
};

struct DfsResult {
    long long depth;
    int point;

    DfsResult(long long d, int p)
        :depth(d), point(p)
    {
    }

    bool operator<(const DfsResult &other) const
    {
        return this->depth < other.depth;
    }
};

DfsResult DFS(const vector<vector<Edge>> &graph, int point, vector<bool> &state, long long length)
{
    if (state[point]) {
        return DfsResult(-1, -1);
    }
    state[point] = true;
    DfsResult r(length, point);
    for (const Edge &e: graph[point]) {
        DfsResult t = DFS(graph, e.to, state, length + e.length);
        r = max(r, t);
    }
    state[point] = false;
    return r;
}

long long diameter(const vector<vector<Edge>> &graph)
{
    vector<bool> state(graph.size(), false);
    int p1 = DFS(graph, 1, state, 0).point;
    for (auto t: state) {
        t = false;
    }
    return DFS(graph, p1, state, 0).depth;
}

int main()
{
    int n;
    scanf("%d", &n);
    vector<vector<Edge>> graph(n + 1);
    long long walk_length = 0;
    for (int i = 0; i < n - 1; i++) {
        int a, b, l;
        scanf("%d%d%d", &a, &b, &l);
        graph[a].emplace_back(b, l);
        graph[b].emplace_back(a, l);
        walk_length += (long long)l * 2;
    }
    walk_length -= diameter(graph);
    printf("%lld\n", walk_length);
    return 0;
}

