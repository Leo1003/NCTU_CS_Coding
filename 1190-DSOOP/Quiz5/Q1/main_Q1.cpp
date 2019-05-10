#include "Array.h"
#include<iostream> 
using namespace std;

int main()
{
    int c, len;
    cin >> c;
    if (c == 1)
    {
        int s1, s2;
        cin >> len;
        int *ptr = new int[len];
        for (int i = 0; i < len; i++)
            cin >> ptr[i];
        cin >> s1 >> s2;

        Array<int> a1(ptr, len);
        a1.display();
        cout << a1.calculate() << endl;
        cout << a1.seek_index(s1) << "_" << a1.seek_index(s2) << endl;
        cout << a1.find_min() << "_" << a1.find_max()<<endl;
        
        delete[] ptr;
    }
    else
    {
        double s1, s2;
        cin >> len;
        double *ptr = new double[len];
        for (int i = 0; i < len; i++)
            cin >> ptr[i];
        cin >> s1 >> s2;

        Array<double> a2(ptr, len);
        a2.display();
        cout << a2.calculate() << endl;
        cout << a2.seek_index(s1) << "_" << a2.seek_index(s2) << endl;
        cout << a2.find_min() << "_" << a2.find_max()<<endl;
        
        delete[] ptr;
    }
}
