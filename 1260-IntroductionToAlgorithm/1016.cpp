#include <algorithm>
#include <iostream>
#include <queue>
#include <vector>
using namespace std;

struct Assignment {
    unsigned int duetime, score;

    Assignment()
    {
    }

    Assignment(int d, int s)
        : duetime(d)
        , score(s)
    {
    }

    friend bool operator<(const Assignment &a, const Assignment &b);
    friend istream &operator>>(istream &s, Assignment &self);
};

bool operator<(const Assignment &a, const Assignment &b)
{
    return a.duetime < b.duetime;
}
istream &operator>>(istream &s, Assignment &self)
{
    s >> self.score >> self.duetime;
    return s;
}

struct min_score {
    bool operator()(const Assignment &a, const Assignment &b)
    {
        return a.score > b.score;
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

    long long total_score = 0;
    priority_queue<Assignment, vector<Assignment>, min_score> works;
    for (const Assignment &asmt : a) {
        works.push(asmt);
        total_score += asmt.score;
        if (asmt.duetime < works.size()) {
            const Assignment &dropped = works.top();
            total_score -= dropped.score;
            works.pop();
        }
    }
    cout << total_score << endl;

    return 0;
}
