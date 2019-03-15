#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;

#define VECTOR_MAX  (10000000LL + 10LL)

int main()
{
    long long T;
    scanf("%lld", &T);
    while (T--) {
        unsigned n;
        long long l, r;
        scanf("%u%lld%lld", &n, &l, &r);
        vector<bool> v(min(r - l + 1, VECTOR_MAX));
        bool skipflag = false;
        for (unsigned i = 0; i < n; i++) {
            long long x;
            scanf("%lld", &x);

            if (x < l || x > r) {
                skipflag = true;
            }
            if (skipflag) {
                continue;
            }
            if (x >= (l + (long long)v.size())) {
                continue;
            }
            v.at(x - l) = true;
        }
        if (skipflag) {
            printf("^v^\n");
        } else {
            for (unsigned i = 0; i < v.size(); i++) {
                if (v[i] == false) {
                    printf("%lld\n", l + (long long)i);
                    break;
                }
                if (i == v.size() - 1) {
                    printf("^v^\n");
                }
            }
        }
    }
    return 0;
}

