#include "Mytime.h"

using namespace std;

int main()
{
    int judge = 0, h1, m1, h2, m2;
    int multiplier1, multiplier2;

    cin >> judge >> h1 >> m1 >> h2 >> m2 >> multiplier1 >> multiplier2;
    Mytime timeA(h1, m1), timeB(h2, m2), timeC;
    cout << timeA << "\n"
         << timeB << "\n"
         << timeC << endl;

    timeC = timeA + timeB + Mytime(1, 2) + timeC;
    cout << "timeA + timeB\t" << timeC << endl;

    timeC = timeA * multiplier1;
    cout << "timeA * mult1\t" << timeC << endl;

    timeC = multiplier2 * timeB;
    cout << "mult2 * timeB\t" << timeC << endl;

    if (judge == 1)
    {
        cout << "timeC\t" << timeC++ << endl;
        cout << "timeC\t" << ++timeC << endl;
    }
}