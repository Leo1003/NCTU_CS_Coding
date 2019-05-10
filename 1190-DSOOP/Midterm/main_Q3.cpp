#include <iostream>
#include "Transform.h"

using namespace std;

int main()
{
    double x, y, c, q, i, j;
    cin >> x >> y >> i >> j;
    Transform T(x, y);
    T.set_polar(&c, &q);
    cout << c << "_" << q << endl;
    Distance D(i, j);
    cout << D.get_distance(T) << endl;
}
