#include <iostream>

using namespace std;

int main(){
    long long q, m, s, l, ans, pad, k, over;
    cin >> q >> m >> s >> l;
    pad = (l + m - 1) / m;
    over = q * (pad * m - l);
    if(s <= over){
        cout << pad * q << "\n";
    }else{
        cout << pad * q + (s - over + m - 1) / m << "\n";
    }
}