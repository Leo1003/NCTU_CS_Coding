#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GOOD_CONST 425

bool isgood(unsigned int n)
{
    int d[10];
    for (int i = 0; i < 10; i++) {
        d[i] = n % 10;
        n /= 10;
    }
    for (int i = 8; i >= 0; i--) {
        d[i] += d[i + 1];
    }
    int total = 0;
    for (int i = 0; i < 10; i++) {
        total += d[i];
    }
    return total == GOOD_CONST;
}

bool isbad(unsigned int n)
{
    int d[10];
    for (int i = 0; i < 10; i++) {
        d[i] = n % 10;
        n /= 10;
    }
    bool d_1 = false, d_3 = false, d_5 = false;
    for (int i = 0; i < 10; i++) {
        switch (d[i]) {
            case 1:
                d_1 = true;
                break;
            case 3:
                d_3 = true;
                break;
            case 5:
                d_5 = true;
                break;
        }
    }
    return (d_1 && d_3 && d_5);
}

bool isugly(unsigned int n)
{
    while (n % 2 == 0) {
        n /= 2;
    }
    while (n % 3 == 0) {
        n /= 3;
    }
    while (n % 5 == 0) {
        n /= 5;
    }
    return n == 1;
}

int main()
{
    unsigned int n;
    while(~scanf("%u", &n)) {
        if (isgood(n)) 
            printf("Good");
        else 
            printf("Not good");

        printf(", ");

        if (isbad(n))
            printf("Bad");
        else 
            printf("Not bad");

        printf(", ");

        if (isugly(n))
            printf("Ugly");
        else 
            printf("Not ugly");
        printf("\n");
    }
    return 0;
}
