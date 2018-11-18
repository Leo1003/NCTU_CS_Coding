#include<stdio.h>
#include<string.h>

#pragma GCC optimize "no-stack-protector,no-exceptions"

#define ARR_SIZE 100000
#ifdef _WIN32
#define getcx getchar
#else
#define getcx getchar_unlocked
#endif

unsigned long long ans = 0;
int v[ARR_SIZE];
int tmp[ARR_SIZE];

inline long long inp()
{
    long long n = 0;
    int ch = getcx();
    int sign = 1;
    while (ch < '0' || ch > '9')
    {
        if (ch == '-')
            sign = -1;
        ch = getcx();
    }
    while (ch >= '0' && ch <= '9')
        n = (n << 3) + (n << 1) + ch - '0', ch = getcx();
    return n * sign;
}

void merge_sort(int l, int r)
{
    if (l == r)
        return;
    //Get the middle location
    int m = (l + r) / 2;
    //Split into two parts
    merge_sort(l, m);
    merge_sort(m + 1, r);

    //Temporary array and its pointer
    int tp = 0;
    //Pointer left and right
    int pl = l, pr = m + 1;

    //Compare left and right values
    while (pl <= m && pr <= r) {
        if (v[pl] <= v[pr]) {
            //Append left value into tmp
            tmp[tp++] = v[pl++];
        } else {
            //Append right value into tmp
            tmp[tp++] = v[pr++];
            //Calculate inversions count
            ans += m - pl + 1;
        }
    }

    //Handling remaining values
    while (pl <= m) {
        //Put remaining left value into tmp
        tmp[tp++] = v[pl++];
    }
    while (pr <= r) {
        //Put remaining right value into tmp
        tmp[tp++] = v[pr++];
    }
    //Copy temporary values back to array
    //memcpy(v + l, tmp, (r - l + 1) * sizeof(int));
    for (int i = 0; i < tp; i++) {
        v[l + i] = tmp[i];
    }
}

int main()
{
    int T = 0;
    while (++T) {
        int n = inp();
        if (n == 0)
            break;
        //Reset answer
        ans = 0;

        for (int i = 0; i < n; i++) {
            v[i] = inp();
        }
        merge_sort(0, n - 1);
        printf("Case #%d: %llu\n", T, ans);
    }
    return 0;
}
