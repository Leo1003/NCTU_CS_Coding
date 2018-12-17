#include <stdio.h>
#include <string.h>

int main() 
{
    while (1) {
        int n;
        char cmd[8];
        scanf("%d", &n);
        if (!n) {
            break;
        }
        int top = 1, north = 2, west = 3, tmp;
        while (n--) {
            scanf("%7s", cmd);
            if (!strcmp(cmd, "north")) {
                tmp = top;
                top = 7 - north;
                north = tmp;
            } else if (!strcmp(cmd, "south")) {
                tmp = top;
                top = north;
                north = 7 - tmp;
            } else if (!strcmp(cmd, "west")) {
                tmp = top;
                top = 7 - west;
                west = tmp;
            } else if (!strcmp(cmd, "east")) {
                tmp = top;
                top = west;
                west = 7 - tmp;
            }
        }
        printf("%d\n", top);
    }
    return 0;
}
