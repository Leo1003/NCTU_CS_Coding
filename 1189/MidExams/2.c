#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    double n;
    scanf("%lf", &n);

    double x = n;
    double y = 1.0;
    while (1) {
        double q = x / y;
        double avg = (y + q) / 2.0;
        printf("%.5lf_%.5lf_%.5lf_%.5lf", x, y, q, avg);
        if (fabs(avg - y) < 0.00001 * y) {
            break;
        }
        printf("\n");
        y = avg;
    }
    return 0;
}
