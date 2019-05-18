#include <algorithm>
#include <iostream>
#include <limits.h>

using namespace std;

long long x1, y1, x2, y2, x, y, trees;

inline bool in_area(long long x, long long y)
{
    return x >= x1 and y >= y1 and x <= x2 and y <= y2;
}

int main()
{
    long long x, y;
    cin >> x >> y >> x1 >> y1 >> x2 >> y2;
    trees = __gcd(x, y);
    ::x = x / trees;
    ::y = y / trees;
    --trees;
    if (trees == 0) {
        cout << "Yes\n";
    } else {
        if (in_area(::x, ::y)) {
            if (in_area(x - ::x, y - ::y)) {
                cout << "Yes" << endl;
            } else {
                cout << "No" << endl;
                long long ans = min(x2 / ::x, y2 / ::y) + 1;
                cout << ::x * ans << " " << ::y * ans << endl;
            }
        } else {
            cout << "No" << endl
                 << ::x << " " << ::y << endl;
        }
    }
}