#include <algorithm>
#include <stdio.h>
#include <queue>
#include <vector>
#define MODULE 524287LL
using namespace std;

int main()
{
    priority_queue<int, vector<int>, greater<int>> q;
    int n;
    long long cost = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        q.push(t);
    }
    while (q.size() > 1) {
        int a = q.top();
        q.pop();
        int b = q.top();
        q.pop();
        int new_val = a + b;
        cost = (cost + new_val) % MODULE;
        q.push(new_val);
    }
    printf("%lld\n", cost);
    return 0;
}
