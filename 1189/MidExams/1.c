#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    float e, ans = 1.0, n = 1.0, i = 1;
    scanf("%f", &e);

    while (1) {
        n /= i;
        i++;
        if (n < e) {
            break;
        }
        ans += n;
    }
    printf("%f", ans);
    return 0;
}
