#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

struct Point {
    Point(size_t id, int x, int y)
        : id(id)
        , rank(0)
        , x(x)
        , y(y)
    {
    }

    bool operator<(const Point &other) const
    {
        return (this->x == other.x ? this->y < other.y : this->x < other.x);
    }

    size_t id, rank;
    int x, y;
};

bool id_cmp(const Point &a, const Point &b)
{
    return a.id < b.id;
}

void mergesort(vector<Point>::iterator begin, vector<Point>::iterator end)
{
    if (begin + 1 >= end) {
        return;
    }
    vector<Point>::iterator mit = begin + distance(begin, end) / 2;
    mergesort(begin, mit);
    mergesort(mit, end);
    vector<Point>::const_iterator p1 = begin, p2 = mit;
    vector<Point> tmp;
    tmp.reserve(distance(begin, end));
    size_t cnt = 0;

    while (p1 < mit && p2 < end) {
        if (p1->y <= p2->y) {
            tmp.push_back(*p1++);
            cnt++;
        } else {
            tmp.push_back(*p2++);
            tmp.back().rank += cnt;
        }
    }
    while (p1 < mit) {
        tmp.push_back(*p1++);
    }
    while (p2 < end) {
        tmp.push_back(*p2++);
        tmp.back().rank += cnt;
    }
    copy(tmp.begin(), tmp.end(), begin);
}

int main()
{
    vector<Point> locs, stk;
    int n;
    scanf("%d", &n);
    locs.reserve(n);
    stk.reserve(n);
    for (int i = 0; i < n; i++) {
        int x, y;
        scanf("%d%d", &x, &y);
        locs.emplace_back(i, x, y);
    }
    sort(locs.begin(), locs.end());

    mergesort(locs.begin(), locs.end());
    
    sort(locs.begin(), locs.end(), id_cmp);
    for (const Point &p: locs) {
        printf("%zu ", p.rank);
    }
    printf("\n");

    return 0;
}

