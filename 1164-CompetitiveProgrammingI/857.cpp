#include <stdio.h>
#include <vector>
using namespace std;

int main()
{
    int n, q;
    scanf("%d%d", &n, &q);
    vector<int> axor;
    axor.reserve(n + 1);

    int last = 0;
    axor.push_back(last);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        last ^= t;
        axor.push_back(last);
    }
    for (int i = 0; i < q; i++) {
        int l, r;
        scanf("%d%d", &l, &r);
        printf("%d\n", axor[l - 1] ^ axor[r]);
    }
    return 0;
}
