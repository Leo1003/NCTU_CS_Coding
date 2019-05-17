#include <stdio.h>

struct range {
    int row_l, row_r;
    int col_l, col_r;
    long long val_l, val_r;
};

struct range range_new(int n)
{
    struct range rg;
    rg.row_l = 0;
    rg.row_r = (1 << n) - 1;
    rg.col_l = 0;
    rg.col_r = (1 << n) - 1;
    rg.val_l = 0;
    rg.val_r = (1LL << (2 * n)) - 1;
    return rg;
}

long long lookup_id(int r, int c, struct range rg)
{
    //fprintf(stderr, "row = [%d, %d]; column = [%d, %d]; value = [%lld, %lld]\n", rg.row_l, rg.row_r, rg.col_l, rg.col_r, rg.val_l, rg.val_r);
    if (rg.row_l == rg.row_r && rg.col_l == rg.col_r) {
        return rg.val_l;
    }
    int rm = (rg.row_l + rg.row_r) / 2;
    int cm = (rg.col_l + rg.col_r) / 2;
    long long v4 = (rg.val_r - rg.val_l + 1) / 4;
    long long base_l = rg.val_l;
    if (r <= rm) {
        if (c <= cm) {
            rg.row_r = rm;
            rg.col_r = cm;
            rg.val_r = base_l + v4 - 1;
            return lookup_id(r, c, rg);
        } else {
            rg.row_r = rm;
            rg.col_l = cm + 1;
            rg.val_l = base_l + v4;
            rg.val_r = base_l + v4 * 2 - 1;
            return lookup_id(r, c, rg);
        }
    } else {
        if (c <= cm) {
            rg.row_l = rm + 1;
            rg.col_r = cm;
            rg.val_l = base_l + v4 * 2;
            rg.val_r = base_l + v4 * 3 - 1;
            return lookup_id(r, c, rg);
        } else {
            rg.row_l = rm + 1;
            rg.col_l = cm + 1;
            rg.val_l = base_l + v4 * 3;
            return lookup_id(r, c, rg);
        }
    }
}

struct range lookup_range(long long id, struct range rg)
{
    if (rg.row_l == rg.row_r && rg.col_l == rg.col_r) {
        return rg;
    }
    int rm = (rg.row_l + rg.row_r) / 2;
    int cm = (rg.col_l + rg.col_r) / 2;
    long long vm = (rg.val_l + rg.val_r) / 2;
    if (id <= vm) {
        long long vm2 = (rg.val_l + vm) / 2;
        if (id <= vm2) {
            rg.row_r = rm;
            rg.col_r = cm;
            rg.val_r = vm2;
            return lookup_range(id, rg);
        } else {
            rg.row_r = rm;
            rg.col_l = cm + 1;
            rg.val_l = vm2 + 1;
            rg.val_r = vm;
            return lookup_range(id, rg);
        }
    } else {
        long long vm2 = (vm + 1 + rg.val_r) / 2;
        if (id <= vm2) {
            rg.row_l = rm + 1;
            rg.col_r = cm;
            rg.val_l = vm + 1;
            rg.val_r = vm2;
            return lookup_range(id, rg);
        } else {
            rg.row_l = rm + 1;
            rg.col_l = cm + 1;
            rg.val_l = vm2 + 1;
            return lookup_range(id, rg);
        }
    }
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        int n, q;
        scanf("%d%d", &n, &q);
        for (int i = 0; i < q; i++) {
            int type;
            scanf("%d", &type);
            if (type == 1) {
                int r, c;
                struct range rg = range_new(n);
                scanf("%d%d", &r, &c);
                printf("%lld\n", lookup_id(r - 1, c - 1, rg) + 1);
            } else if (type == 2) {
                long long id;
                struct range rg = range_new(n);
                scanf("%lld", &id);
                struct range ans = lookup_range(id - 1, rg);
                printf("%d %d\n", ans.row_l + 1, ans.col_l + 1);
            }
        }
    }
    return 0;
}
