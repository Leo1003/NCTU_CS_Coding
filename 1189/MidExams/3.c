#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define KUTI    10000000
#define LAKH    100000
#define HAJAR   1000
#define SHATA   100

#define LEVEL(n, lv, name) \
do { \
    if (n / lv) { \
        bangla(n / lv); \
        printf("%s", #name); \
        n %= lv; \
    } \
} while(0) 

int print_ = 0;

void bangla(unsigned long long num)
{
    LEVEL(num, KUTI, _kuti);
    LEVEL(num, LAKH, _lakh);
    LEVEL(num, HAJAR, _hajar);
    LEVEL(num, SHATA, _shata);
    if (num) {
        if (print_++) {
            printf("_");
        }
        printf("%lld", num);
    }
}

int main()
{
    unsigned long long n;
    scanf("%lld", &n);
    bangla(n);
    return 0;
}
