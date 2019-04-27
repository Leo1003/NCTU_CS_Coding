#include <stdio.h>
#include <string.h>

int main()
{
    int l, x, denied = 0, current = 0;
    scanf("%d%d", &l, &x);
    for (int i = 0; i < x; i++) {
        char cmd[101];
        int n;
        scanf("%100s%d", cmd, &n);
        if (strcmp("enter", cmd) == 0) {
            if (current + n > l) {
                denied++;
            } else {
                current += n;
            }
        } else if (strcmp("leave", cmd) == 0) {
            current -= n;
        }
    }
    printf("%d\n", denied);
    return 0;
}
