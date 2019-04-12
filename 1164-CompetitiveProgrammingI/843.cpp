#include <algorithm>
#include <limits.h>
#include <map>
#include <stdio.h>
#include <string.h>
using namespace std;

map<int, int> chocolate;

int get_left(map<int, int>::iterator it) {
    if (it == chocolate.begin()) {
        return 0;
    }
    it--;
    return it->first;
}

int get_next_height(map<int, int>::iterator it) {
    if (it == chocolate.begin()) {
        return INT_MAX;
    }
    it--;
    return it->second;
}

long long eat(int x, int y)
{
    long long eaten = 0;
    int height = 0;
    
    map<int, int>::iterator it = chocolate.lower_bound(x);

    if (it == chocolate.end()) {
        height = 0;
    } else {
        height = it->second;
    }

    // Check if at non-eaten area
    if (height >= y) {
        return 0;
    }
    
    int x_pos = x;
    do {
        int left = get_left(it);

        eaten += (long long)(x_pos - left) * (long long)(y - height);

        height = get_next_height(it);
        x_pos = left; 

        // Remove point if it's inside eaten area
        map<int, int>::iterator pvit = it;
        if (it != chocolate.begin()) {
            it--;
        }
        if (pvit != chocolate.end() && pvit->second <= y && pvit->first <= x) {
            chocolate.erase(pvit);
        }
    } while (height < y && x_pos > 0);

    chocolate[x] = y;

    return eaten;
}

int main()
{
    int q;
    long long n, m, left = 0;
    scanf("%lld%lld%d", &n, &m, &q);
    left = n * m;
    
    for (int i = 0; i < q; i++) {
        int x, y;
        scanf("%d%d", &x, &y);
        long long e = eat(x, y);
        left -= e;
        if (e <= 0) {
            printf("QAQ\n");
        } else {
            printf("%lld\n", e);
        }
    }
    printf("%lld\n", left);
    return 0;
}
