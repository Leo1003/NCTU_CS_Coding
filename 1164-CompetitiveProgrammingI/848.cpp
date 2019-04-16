#include <algorithm>
#include <map>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define eprintf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else 
# define eprintf(fmt, ...)
#endif

typedef map<int, int>::iterator mapiiIter;

inline int get_height(map<int, int> &m, int x)
{
    mapiiIter it = m.lower_bound(x);
    if (it->first == x) {
        return it->second;
    } else {
        if (it != m.begin()) {
            it--;
            return it->second;
        } else {
            return 0;
        }
    }
}

inline void safe_erase(map<int, int> &m, mapiiIter &iter)
{
    if (iter != m.end()) {
        eprintf("Remove: (%d, %d)\n", iter->first, iter->second);
        m.erase(iter);
        iter = m.end();
    }
}

int main()
{
    int n;
    scanf("%d", &n);
    map<int, int> edge;
    for (int i = 0; i < n; i++) {
        int x, h, w;
        scanf("%d%d%d", &x, &h, &w);
        int r = x + w;

        int r_h = get_height(edge, r);
        if (r_h < h) {
            eprintf("Insert: (%d, %d)\n", r, r_h);
            edge[r] = r_h;
        }
        int l_h = get_height(edge, x);
        if (l_h < h) {
            eprintf("Insert: (%d, %d)\n", x, h);
            edge[x] = h;
        }

        mapiiIter lit = edge.lower_bound(x), rit = edge.upper_bound(r);

        mapiiIter to_remove = edge.end();
        int prev_h = l_h;
        for (mapiiIter it = lit; it != rit; it++) {
            safe_erase(edge, to_remove);
            
            if (it->first < x) {
                prev_h = it->second;
                continue;
            }
            if (it->first == x || it->first == r) {
                prev_h = it->second;
                continue;
            }

            if (it->second < h) {
                // Determine uplift
                eprintf("Uplift: (%d, %d) -> (%d, %d)\n", it->first, it->second, it->first, h);
                it->second = h;
            }
            // Determine remove
            if (it->second == prev_h) {
                to_remove = it;
            } else {
                // Update previous height
                prev_h = it->second;
            }
        }
        safe_erase(edge, to_remove);
    }
    long long area = 0, length = 0;
    int last_x = 0, last_y = 0;
    for (pair<int, int> p: edge) {
        eprintf("Point: (%d, %d)\n", p.first, p.second);
        if (last_y > 0) {
            length += (long long)abs(p.first - last_x);
        }
        length += (long long)abs(p.second - last_y);
        area += (long long)last_y * (long long)abs(p.first - last_x);
        last_x = p.first;
        last_y = p.second;
    }
    printf("%lld %lld\n", length, area);
    return 0;
}
