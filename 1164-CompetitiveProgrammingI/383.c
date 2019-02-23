#include <stdio.h>

int main()
{
    int z = 0, o = 0;
    char ch;
    while ((ch = getchar()) != EOF) {
        switch (ch) {
            case '0':
                z++;
                break;
            case '1':
                o++;
                break;
            default:
                break;
        }       
    }
    printf("%d %d\n", z, o);
    return 0;
}
