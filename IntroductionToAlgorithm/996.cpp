#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

struct num {
    long long take;
    long long ntake;

    void swap_take() {
        swap(take, ntake);
    }
};

int main()
{
    int n;
    scanf("%d", &n);
    vector<int> a(n);
    vector<num> head(n), nhead(n);
    
    for (int i = 0; i < n; i++) {
        scanf("%d", &a[i]);
    }
    if (n == 1) {
        printf("%d\n", a[0]);
        return 0;
    }
    head[0].take = a[0];
    head[0].ntake = 0;
    nhead[0].take = 0;
    nhead[0].ntake = 0;
    if (n >= 2) {
        head[1].take = head[0].take;
        head[1].ntake = head[0].take;
        nhead[1].take = a[1];
        nhead[1].ntake = 0;
    }
    for (int i = 2; i < n; i++) {
        head[i].take = head[i - 1].ntake + a[i];
        head[i].ntake = max(head[i - 1].ntake, head[i - 1].take);
        nhead[i].take = nhead[i - 1].ntake + a[i];
        nhead[i].ntake = max(nhead[i - 1].ntake, nhead[i - 1].take);
    }
    
    printf("%lld\n", max(head[n - 1].ntake, max(nhead[n - 1].take, nhead[n - 1].ntake)));
    return 0;
}
