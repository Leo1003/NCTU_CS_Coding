#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <vector>
using namespace std;

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        long long total_value = 0;
        int n, k;
        vector<int> fruits;
        scanf("%d%d", &n, &k);
        fruits.reserve(n);
        for (int i = 0; i < n; i++) {
            int t;
            scanf("%d", &t);
            total_value += t;
            fruits.push_back(t);
        }
        double first_day = total_value / (double)(k + 100) * (double)k;
        if (floor(first_day) != ceil(first_day)) {
            printf("No\n");
            continue;
        }
        long long weight = ceil(first_day);
        vector<long long> backpack(weight + 1);
        for (long long i = 0; i < n; i++) {
            for (long long j = weight; j - fruits[i] >= 0; j--) {
                backpack[j] = max(backpack[j], backpack[j - fruits[i]] + fruits[i]);
            }
        }
        if (backpack[weight] == weight) {
            printf("Yes\n");
        } else {
            printf("No\n");
        }
    }
    return 0;
}
