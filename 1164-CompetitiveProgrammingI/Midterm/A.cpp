#include <iostream>
using namespace std;

int main() 
{
    int t;
    cin >> t;
    for (int i = 0; i < t; i++) {
        int a, b, x;
        cin >> a >> b >> x;
        int h = x - b;
        int day = a - b;
        int c = h / day;
        int left = h % day;
        if (c <= 0) {
            cout << 1 << "\n";
        } else {
            if (left == 0) {
                cout << c << "\n";
            } else {
                cout << c + 1 << "\n";
            }
        }
    }
    return 0;
}

