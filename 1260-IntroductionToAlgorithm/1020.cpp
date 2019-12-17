#include <algorithm>
#include <stdio.h>
#include <queue>
#include <vector>
using namespace std;

int main()
{
    priority_queue<long long, vector<long long>, greater<long long>> q;
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        q.push(t);
    }
    while (q.size() > 1) {
        long long a = q.top();
        q.pop();
        long long b = q.top();
        q.pop();
        long long new_val = (a + b) * 2;
        q.push(new_val);
    }
    printf("%lld\n", q.top());
    return 0;
}
