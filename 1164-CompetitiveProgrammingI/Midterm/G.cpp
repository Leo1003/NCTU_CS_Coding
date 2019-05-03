#include <stdio.h>
#include <map>
using namespace std;

inline bool get_status(map<int, bool>& m, int pos)
{
    auto it = m.lower_bound(pos);
    if (it == m.end() || it->first > pos) {
        if (it == m.begin()) {
            return false;
        }
        return (--it)->second;
    } else {
        return it->second;
    }
}

int solder(map<int, bool>& m, int l, int r, bool sold) 
{
    bool cur = get_status(m, l);
    bool r_cur = get_status(m, r);
    
    if (cur != sold) {
        m[l] = sold;
    }
    if (r_cur != sold) {
        m[r] = r_cur;
    }
    
    int ans = 0;
    int last = l;
    auto it = m.lower_bound(l);
    if (it->first == l) {
        it++;
    }
    auto to_erase_l = m.end();
    auto to_erase_r = m.end();
    while (last < r && it != m.end()) {
        if (cur != sold) {
            ans += it->first - last;
        }
        cur = it->second;
        last = it->first;
        if (it->first < r) {
            if (to_erase_l == m.end()) {
                to_erase_l = it;
            }
            to_erase_r = it;
        }
        it++;
    }
    if (to_erase_l != m.end()) {
        to_erase_r++;
        m.erase(to_erase_l, to_erase_r);
    }
    return ans;
}

int main()
{
    int q;
    scanf("%d", &q);
    map<int, bool> seats;
    for (int i = 0; i < q; i++) {
        int t, l, r;
        scanf("%d%d%d", &t, &l, &r);
        if (t == 1) {
            printf("%d\n", solder(seats, l, r, true));
        } else if (t == 2) {
            printf("%d\n", solder(seats, l, r, false));
        }
    }
    return 0;
}

