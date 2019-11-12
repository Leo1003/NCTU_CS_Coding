#include <algorithm>
#include <unordered_map>
#include <stdio.h>
#include <vector>
#include <utility>
using namespace std;

typedef unordered_map<int, vector<pair<size_t, size_t>>> pair_map;

void insert_pair(pair_map &m, int key, size_t x, size_t y)
{
    m[key].push_back(make_pair(x, y));
}

bool check_dup(const pair<size_t, size_t> &p, size_t x, size_t y)
{
    return (p.first != x) && (p.first != y) && (p.second != x) && (p.second != y);
}

int main()
{
    size_t n;
    bool ans = false;
    scanf("%zu", &n);
    vector<int> A(n);
    for (size_t i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }
    pair_map m;
    m.reserve(4 * n * (n - 1));
    vector<vector<int>> table(n, vector<int>(n));
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            table[i][j] = A[i] + A[j];
            insert_pair(m, table[i][j], i, j);
        }
    }
    for (size_t i = 0; i < n; i++) {
        for (size_t j = i + 1; j < n; j++) {
            if (table[i][j] < 0) {
                continue;
            }
            pair_map::const_iterator it;
            if ((it = m.find(-table[i][j])) != m.cend()) {
                for (const auto &pair: it->second) {
                    if (check_dup(pair, i, j)) {
                        ans = true;
                        goto ans_out;
                    }
                }
            }
        }
    }

ans_out:
    if (ans) {
        printf("1\n");
    } else {
        printf("0\n");
    }

    return 0;
}
