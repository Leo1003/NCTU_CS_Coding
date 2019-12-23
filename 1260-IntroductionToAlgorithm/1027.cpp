#include <algorithm>
#include <limits.h>
#include <stack>
#include <stdio.h>
#include <queue>
#include <vector>
using namespace std;

typedef vector<vector<int>> AdjMatrix;

struct State {
    State()
        :prev(-1), flow(0)
    {}
    int prev, flow;
};

int bfs(AdjMatrix &graph, int start, int end, vector<int> &bt) {
    int n = graph.size();
    vector<State> bfs_state(n);
    bt.clear();
    bfs_state[0].flow = INT_MAX;
    queue<int> bfs;
    bfs.push(start);
    while (!bfs.empty()) {
        int point = bfs.front();
        bfs.pop();
        if (point == end) {
            int curr = point;
            while (curr != -1) {
                bt.push_back(curr);
                curr = bfs_state[curr].prev;
            }
            return bfs_state[end].flow;
        }
        for (int j = 1; j < n; j++) {
            if (bfs_state[j].prev == -1 && graph[point][j] > 0) {
                bfs_state[j].prev = point;
                int prevflow = bfs_state[point].flow;
                bfs_state[j].flow = min(graph[point][j], prevflow);
                bfs.push(j);
            }
        }
    }
    return 0;
}

long long edmonds_karp(AdjMatrix &graph)
{
    int n = graph.size();
    long long total_flow = 0;
    vector<int> bt;
    int flow;
    while ((flow = bfs(graph, 0, n - 1, bt))) {
        total_flow += flow;
        for (size_t i = 0; i < bt.size() - 1; i++) {
            int curr = bt[i];
            int prev = bt[i + 1];
            graph[curr][prev] += flow;
            graph[prev][curr] -= flow;
        }
    }
    return total_flow;
}

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    AdjMatrix graph(n, vector<int>(n, 0));
    
    for (int i = 0; i < m; i++) {
        int a, b;
        scanf("%d%d", &a, &b);

        graph[a][b] += 1;
        graph[b][a] += 1;
    }

    printf("%lld\n", edmonds_karp(graph));
    return 0;
}
