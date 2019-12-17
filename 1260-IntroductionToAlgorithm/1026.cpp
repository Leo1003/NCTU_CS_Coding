#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

typedef vector<vector<int>> GraphTable;

void Floyd_Warshall(const GraphTable &graph, GraphTable &dis)
{
    dis = GraphTable(graph);

    for (size_t i = 0; i < graph.size(); i++) {
        dis[i][i] = 0;
    }

    for (size_t m = 0; m < dis.size(); m++) {
        for (size_t i = 0; i < dis.size(); i++) {
            for (size_t j = 0; j < dis.size(); j++) {
                dis[i][j] = min(dis[i][j], max(dis[i][m], dis[m][j]));
            }
        }
    }
}

int main()
{
    int n, m, q;
    scanf("%d%d%d", &n, &m, &q);
    GraphTable graph(n, vector<int>(n, INT_MAX));
    for (int i = 0; i < m; i++) {
        int f, t, b;
        scanf("%d%d%d", &f, &t, &b);
        graph[f][t] = min(graph[f][t], b);
    }
    GraphTable dis;
    Floyd_Warshall(graph, dis);

    for (int i = 0; i < q; i++) {
        int a, b;
        scanf("%d%d", &a, &b);
        int ans = max(dis[a][b], dis[b][a]);
        if (ans == INT_MAX) {
            ans = -1;
        }
        printf("%d\n", ans);
    }

    return 0;
}

