#include <stdio.h>
#define SIZE 100

void swap(double *a, double *b)
{
    double t = *b;
    *b = *a;
    *a = t;
}

void sort(double v[], int n)
{
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (v[j] > v[j + 1]) {
                swap(v + j, v + j + 1);
            }
        }
    }
}

int main()
{
    int n;
    double v[SIZE];
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        scanf("%lf", &v[i]);
    }
    sort(v, n);
    for (int i = 0; i < n; i++) {
        printf("%.6lg ", v[i]);
    }
    printf("\n");
    return 0;
}

