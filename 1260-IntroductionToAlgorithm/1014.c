#include <limits.h>
#include <stdio.h>

struct Context {
    long long value;
    int begin, end;
};

void ctx_reset(struct Context *ctx, int index)
{
    ctx->value = 0;
    ctx->begin = index;
    ctx->end = index;
}

int main()
{
    int n;
    scanf("%d", &n);

    struct Context mx, cur;
    mx.value = LLONG_MIN;
    mx.begin = -1;
    mx.end = -1;

    ctx_reset(&cur, 0);
    for (int i = 0; i < n; i++) {
        int t;
        scanf("%d", &t);
        cur.value += t;
        cur.end++;

        if (cur.value > mx.value) {
            mx = cur;
        }
        if (cur.value < 0) {
            ctx_reset(&cur, i + 1);
        }
    }

    printf("%d %d\n", mx.begin, mx.end);
    return 0;
}
