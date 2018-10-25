#include <stdio.h>
#include <stdlib.h>
#define MAX(a, b) (a < b ? b : a)
#define MIN(a, b) (a < b ? a : b)

int main()
{
    int n, map[100], capacity = 0, max_h = 0;
    scanf("%d", &n);

    for (int i = 0; i < n; i++) {
        scanf("%d", &map[i]);
        max_h = MAX(max_h, map[i]);
    }

    //Test for every height
    for (int h = 1; h <= max_h; h++) {
        int start = -1;
        for (int i = 0; i < n; i++) {
            if (map[i] >= h) {
                if (start == -1) {
                    start = i;
                } else {
                    capacity += (i - start - 1);
                    start = i;
                }
            }
        }
    }

    printf("%d\n", capacity);
    return 0;
}
