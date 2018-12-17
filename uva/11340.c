#include <stdio.h>
#include <string.h>

int value[256];

int main()
{
    int n;
    scanf("%d\n", &n);
    while (n--) {
        memset(value, 0, sizeof(value));
        int k, l;
        unsigned long long int money = 0;
        scanf("%d\n", &k);
        while (k--) {
            unsigned char c;
            int v;
            scanf("%c %d\n", &c, &v);
            value[c] = v;
        }
        scanf("%d\n", &l);
        while (l--) {
            unsigned char str[10010];
            fgets_unlocked(str, sizeof(str), stdin);
            int i;
            for (i = 0; str[i]; i++) {
                money += value[str[i]];
            }
            /*unsigned char C = getchar_unlocked();
            //if (C == '\n') {
            //    l--;
            //    continue;
            //}
            //money += value[C];*/
        }
        printf("%llu.%02u$\n", money / 100, money % 100);
    }
    return 0;
}
