#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;

struct Edge {
    int a, b, weight;

    Edge(int a, int b, int w)
        :a(a), b(b), weight(w)
    {}
};

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    vector<int> vertex(n);
    vector<Edge> graph;
    for (int i = 0; i < n; i++) {
        scanf("%d", &vertex[i]);
    }
    for (int p = 0; p < m; p++) {
        int i, j, k;
        scanf("%d%d%d", &i, &j, &k);
        graph.emplace_back(i, j, k);
    }

    long long ans = 0;
    vector<long long> distance(n, LLONG_MAX);
    distance[0] = vertex[0];
    for (int t = 0; t < n - 1; t++) {
        bool relaxed = false;
        for (const Edge &e: graph) {
            int a = e.a;
            int b = e.b;
            int weight = e.weight;
            if (distance[a] + weight + vertex[b] < distance[b]) {
                distance[b] = distance[a] + weight + vertex[b];
                relaxed = true;
            }
            if (distance[b] + weight + vertex[a] < distance[a]) {
                distance[a] = distance[b] + weight + vertex[a];
                relaxed = true;
            }
        }
        if (!relaxed) {
            break;
        }
    }

    for (int i = 1; i < n; i++) {
        ans += distance[i];
    }
    printf("%lld\n", ans);
    return 0;
}

