#include <algorithm>
#include <stdio.h>
using namespace std;

int main()
{
    int n;
    scanf("%d", &n);
    int mx = 0;
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        mx = max(mx, t);
    }
    printf("%d\n", mx);
    return 0;
}