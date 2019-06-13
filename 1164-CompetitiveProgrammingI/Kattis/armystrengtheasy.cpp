#include <algorithm>
#include <stdio.h>
using namespace std;

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        int ng, nm, max_g = 0, max_m = 0;
        scanf("%d%d", &ng, &nm);
        int t;
        for (int i = 0; i < ng; i++) {
            scanf("%d", &t);
            max_g = max(max_g, t);
        }
        for (int i = 0; i < nm; i++) {
            scanf("%d", &t);
            max_m = max(max_m, t);
        }
        if (max_m > max_g) {
            printf("MechaGodzilla\n");
        } else {
            printf("Godzilla\n");
        }
    }
    return 0;
}
