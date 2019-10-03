#include <algorithm>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

class SegmentTree {
public:
    SegmentTree(size_t n)
    {
        length = n;
        base = 1;
        size_t l = 1;
        n--;
        while (n) {
            n /= 2;
            base *= 2;
            l++;
        }
        level = l;
        data = vector<int>(base * 2);
        debugf("Level: %zd\n", level);
        debugf("Length: %zd\n", length);
        debugf("Base: %zd\n", base);
    }
    
    size_t size() const
    {
        return length;
    }

    void init_update()
    {
        size_t item = base / 2;
        for (size_t l = 1; l < level; l++) {
            for (size_t x = 0; x < item; x++) {
                debugf("Init: (%zd, %zd)\n", x, l);
                renew(x, l);
            }
            item /= 2;
        }
    }

    int &at(size_t x)
    {
        return level_accessor(x, 0);
    }
    
    void update(size_t x, int value)
    {
        if (level_accessor(x, 0) == value) {
            return;
        }
        level_accessor(x, 0) = value;

        for (size_t l = 1; l < level; l++) {
            size_t div = 1 << l;
            size_t l_x = x / div;
            renew(l_x, l);
        }
    }

    int &operator[](size_t x)
    {
        return level_accessor(x, 0);
    }

    int query(size_t l, size_t r) const
    {
        return query_internal(l, r, 0, level - 1);
    }

    void dump() const 
    {
        debugf("====== SegmentTree Dump ======\n");
        for (int l = level - 1; l >= 0; l--) {
            for (int j = 0; j < (1 << (level - l - 1)); j++) {
                debugf("%d ", const_accessor(j, l));
            }
            debugf("\n");
        }
    }

private:
    inline size_t lbound(size_t x, size_t l) const
    {
        return x * (1 << l);
    }
    inline size_t rbound(size_t x, size_t l) const
    {
        return (x + 1) * (1 << l) - 1;
    }

    void renew(size_t x, size_t l)
    {
        level_accessor(x, l) = calculate(const_accessor(x * 2, l - 1), const_accessor(x * 2 + 1, l - 1));
    }

    int query_internal(size_t l, size_t r, size_t x, size_t lv) const
    {
        size_t lb = lbound(x, lv);
        size_t rb = rbound(x, lv);

        if (l <= lb && rb <= r) {
            return const_accessor(x, lv);
        }

        int ret = 0;
        size_t m = rbound(x * 2, lv - 1);
        if (l <= m) {
            ret = query_internal(l, r, x * 2, lv - 1);
        }
        if (m < r) {
            int data = query_internal(l, r, x * 2 + 1, lv - 1);
            if (ret == 0) {
                ret = data;
            } else {
                ret = calculate(ret, data);
            }
        }
        return ret;
    }

    inline int &level_accessor(size_t x, size_t l)
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        debugf("(%zd, %zd) => %d\n", x, l, id);
        return data[id];
    }
    inline const int &const_accessor(size_t x, size_t l) const
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        debugf("(%zd, %zd) => %d\n", x, l, id);
        return data[id];
    }

    static int calculate(int a, int b)
    {
        return max(a, b);
    }

    size_t length, level, base;
    vector<int> data;
};

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    SegmentTree tree(n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &tree[i]);
    }
    tree.init_update();

    for (int i = 0; i < m; i++) {
        int t, a, b;
        scanf("%d%d%d", &t, &a, &b);
        if (t == 1) {
            //Modify [a] -> b
            tree.update(a, b);
        } else if (t == 2) {
            // Query [a, b]
            printf("%d\n", tree.query(a, b));
        } else {
            abort();
        }
    }
    return 0;
}

