#include <iostream>
using namespace std; 

void calculate(int target, int &x, int &y)
{
    int start = 1, level = 0;

    while (target > start) {
        level++;
        start += level * 6;
    }
    
    x = level;
    y = 0;
    int direction = 5, step = level, current = start;
    while (target != current) {
        if (!step--) {
            direction--;
            step = level;
            step--;
        }
        switch (direction) {
            case 5:
                y--;
                break;
            case 4:
                x--;
                break;
            case 3:
                x--;
                y++;
                break;
            case 2:
                y++;
                break;
            case 1:
                x++;
                break;
            case 0:
                x++;
                y--;
                break;
        }
        current--;
        //cerr << current + 1 << " " << direction <<"-> " << current << ": (" << x << ", " << y << ")" << endl;
    }
}

int main()
{
    int will, x, y;

    while (cin >> will) {
        calculate(will, x, y);
        cout << x << " " << y << endl;
    }
    return 0;
}
