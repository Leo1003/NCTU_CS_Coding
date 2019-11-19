#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;

long long dc(vector<int> &arr, int l, int r)
{
    if (l == r) {
        return 0;
    }
    int mid = (l + r) / 2;
    long long lans = dc(arr, l, mid);
    long long rans = dc(arr, mid + 1, r);

    long long ans = 0;
    int pl = l, pr = mid + 1;
    vector<int> tmp;
    while (pl <= mid && pr <= r) {
        if (arr[pl] <= arr[pr]) {
            tmp.push_back(arr[pl]);
            pl++;
        } else {
            tmp.push_back(arr[pr]);
            ans += mid - pl + 1;
            pr++;
        }
    }
    while (pl <= mid) {
        tmp.push_back(arr[pl]);
        pl++;
    }
    while (pr <= r) {
        tmp.push_back(arr[pr]);
        pr++;
    }
    for (int i = l; i <= r; i++) {
        arr[i] = tmp[i - l];
    }
    return lans + rans + ans;
}

int main()
{
    int n;
    scanf("%d", &n);
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &arr[i]);
    }
    printf("%lld\n", dc(arr, 0, n - 1));
    return 0;
}