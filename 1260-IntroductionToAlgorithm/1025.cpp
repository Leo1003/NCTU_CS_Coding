#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <queue>
#include <vector>
#include <utility>
using namespace std;

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

typedef pair<long long, int> state;
struct Edge {
    int from, to;
    long long weight;

    Edge(int f, int t, int w)
        :from(f), to(t), weight(w)
    {
    }
};

struct Point {
    vector<Edge> edges;
    vector<Edge> edges_rev;
    bool updated = false;
};

void init_dis(vector<long long> &dis, size_t n, int self)
{
    dis.resize(n);
    for (size_t i = 0; i < dis.size(); i++) {
        if ((int)i == self) {
            dis[i] = 0;
        } else {
            dis[i] = LLONG_MAX;
        }
    }
}

void dijkstra(vector<Point> &graph, int source, vector<long long> &dis, bool reverse = false)
{
    Point &s = graph[source];
    if (s.updated) {
        return;
    }
    init_dis(dis, graph.size(), source);

    priority_queue<state, vector<state>, greater<state>> q;
    vector<bool> visited(graph.size(), false);
    q.push(make_pair(0, source));
    while (!q.empty()) {
        int a = q.top().second;
        q.pop();

        if (visited[a]) {
            continue;
        }

        if (reverse) {
            for (Edge &e: graph[a].edges_rev) {
                int b = e.from;
                if (dis[b] > dis[a] + e.weight) {
                    dis[b] = dis[a] + e.weight;
                    q.push(make_pair(dis[b], b));
                }
            }
        } else {
            for (Edge &e: graph[a].edges) {
                int b = e.to;
                if (dis[b] > dis[a] + e.weight) {
                    dis[b] = dis[a] + e.weight;
                    q.push(make_pair(dis[b], b));
                }
            }
        }
        visited[a] = true;
    }
}

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);

    vector<Point> graph(n + 1);
    for (int e = 0; e < m; e++) {
        int i, j, k;
        scanf("%d%d%d", &i, &j, &k);

        graph[i].edges.emplace_back(i, j, k);
        graph[j].edges_rev.emplace_back(i, j, k);
    }

    long long ans = 0;
    vector<long long> dis;
    dijkstra(graph, 0, dis);
    for (size_t i = 1; i < graph.size(); i++) {
        debugf("0 -> %zu = %lld\n", i, dis[i]);
        ans += dis[i];
    }

    dijkstra(graph, 0, dis, true);
    for (size_t i = 1; i < graph.size(); i++) {
        debugf("%zu -> 0 = %lld\n", i, dis[i]);
        ans += dis[i];
    }
    printf("%lld\n", ans);
    return 0;
}

