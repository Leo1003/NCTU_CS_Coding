#include <stdio.h>

const int regs[] = {2, 3, 5, 7, 11, 13, 17, 19};

int main()
{
    int reg[8];
    for (int i = 0; i < 8; i++) {
        scanf("%d", &reg[i]);
    }
    int sum = 0;
    int level = 1;
    for (int i = 0; i < 8; i++) {
        sum += (regs[i] - reg[i] - 1) * level;
        level *= regs[i];
    }
    printf("%d\n", sum);

    return 0;
}
