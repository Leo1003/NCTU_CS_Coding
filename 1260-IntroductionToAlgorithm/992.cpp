#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

struct paired_num {
    paired_num(int sum, size_t x, size_t y)
        : sum(sum)
        , x(x)
        , y(y)
    {
    }

    bool index_dup(const paired_num &other) const
    {
        return (this->x == other.x) || (this->x == other.y) || (this->y == other.x) || (this->y == other.y);
    }

    bool operator<(const paired_num &other) const
    {
        return this->sum < other.sum;
    }
    friend bool operator<(const paired_num &self, const int other)
    {
        return self.sum < other;
    }
    friend bool operator<(const int other, const paired_num &self)
    {
        return other < self.sum;
    }

    int sum;
    size_t x;
    size_t y;
};

int main()
{
    size_t n;
    bool ans = false;
    scanf("%zu", &n);
    vector<int> A(n);
    for (size_t i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }

    vector<paired_num> pairvec;
    pairvec.reserve(n * (n - 1) / 2);

    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            pairvec.emplace_back(A[i] + A[j], i, j);
        }
    }
    sort(pairvec.begin(), pairvec.end());

    vector<paired_num>::const_iterator positive, zero;
    positive = upper_bound(pairvec.cbegin(), pairvec.cend(), 0);
    zero = lower_bound(pairvec.cbegin(), pairvec.cend(), 0);

    for (auto it = pairvec.begin(); !ans && it != positive; it++) {
        auto it2 = lower_bound(zero, pairvec.cend(), -(it->sum));
        auto it2end = upper_bound(zero, pairvec.cend(), -(it->sum));

        while (!ans && it2 != it2end) {
            if (it2 != pairvec.cend()) {
                if (!it->index_dup(*it2)) {
                    ans = true;
                }
            }
            it2++;
        }
    }

    if (ans) {
        printf("1\n");
    } else {
        printf("0\n");
    }

    return 0;
}

