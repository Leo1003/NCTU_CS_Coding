#include <algorithm>
#include <iostream>
#include <limits.h>
#include <queue>
#include <vector>
using namespace std;

struct Assignment {
    int duetime, spendtime;

    Assignment()
    {
    }

    Assignment(int d, int t)
        : duetime(d)
        , spendtime(t)
    {
    }

    friend bool operator<(const Assignment &a, const Assignment &b)
    {
        return (a.duetime == b.duetime ? a.spendtime < b.spendtime : a.duetime < b.duetime);
    }

    friend istream &operator>>(istream &s, Assignment &self)
    {
        s >> self.duetime >> self.spendtime;
        return s;
    }
};

struct max_spent {
    bool operator()(const Assignment &a, const Assignment &b)
    {
        return a.spendtime < b.spendtime;
    }
};

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n;
    cin >> n;
    vector<Assignment> a(n);
    for (int i = 0; i < n; i++) {
        cin >> a[i];
    }
    sort(a.begin(), a.end());

    long long time = 0;
    priority_queue<Assignment, vector<Assignment>, max_spent> works;
    for (const Assignment &asmt : a) {
        works.push(asmt);
        time += asmt.spendtime;
        if (asmt.duetime < time) {
            const Assignment &dropped = works.top();
            time -= dropped.spendtime;
            works.pop();
        }
    }
    cout << works.size() << endl;

    return 0;
}
