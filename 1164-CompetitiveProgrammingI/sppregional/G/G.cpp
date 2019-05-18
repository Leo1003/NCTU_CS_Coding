#include <iostream>

using namespace std;

int main(){
    long long n, k, last=0, now, ans=0;
    cin >> n >> k;
    while (k--)
    {
        cin >> now;
        ans += (now - last) * (n - now + 1);
        last = now;
    }
    cout << ans << "\n";
}