#include <stdio.h>
#include <string.h>

struct charge {
    double day;
    double even;
    double night;
};

struct charge charging[5] = {
    { 0.10, 0.06, 0.02 },
    { 0.25, 0.15, 0.05 },
    { 0.53, 0.33, 0.13 },
    { 0.87, 0.47, 0.17 },
    { 1.44, 0.80, 0.30 }
};

int timestamp(int h, int m)
{
    return (h * 60 + m);
}

int take(int *much, int limit)
{
    if (*much < 0) {
        return 0;
    }
    int n = (*much >= limit ? limit : *much);
    *much -= n;
    return n;
}

int main()
{
    while (1) {
        char step, name[10 + 1];
        int from, to, h1, m1, h2, m2;
        do {
            scanf("%c", &step);
            if (step == '#') {
                return 0;
            }
        } while (step < 'A' || step > 'Z');
        scanf("%10s", name);
        scanf("%d%d%d%d", &h1, &m1, &h2, &m2);

        from = timestamp(h1, m1);
        to = timestamp(h2, m2);

        int day = 0, even = 0, night = 0;
        int interval = to - from;
        if (interval <= 0) {
            interval += 24 * 60;
        }
        if (from >= 8 * 60 && from < 18 * 60) {
            /*fprintf(stderr, "day\n");*/
            day += take(&interval, 18 * 60 - from);
            even += take(&interval, 4 * 60);
            night += take(&interval, 10 * 60);
            day += interval;
        } else if (from >= 18 * 60 && from < 22 * 60) {
            /*fprintf(stderr, "even\n");*/
            even += take(&interval, 22 * 60 - from);
            night += take(&interval, 10 * 60);
            day += take(&interval, 10 * 60);
            even += interval;
        } else {
            /*fprintf(stderr, "night\n");*/
            if (from < 8 * 60) {
                from += 24 * 60;
            }
            night += take(&interval, 32 * 60 - from);
            day += take(&interval, 10 * 60);
            even += take(&interval, 4 * 60);
            night += interval;
        }
        
        int s = step - 'A';
        double cost = charging[s].day * day + charging[s].even * even + charging[s].night * night;
        printf("%10s% 6d% 6d% 6d  %c% 8.02lf\n", name, day, even, night, step, cost);
    }
    return 0;
}
