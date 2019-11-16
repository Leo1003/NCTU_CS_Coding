#include <algorithm>
#include <limits.h>
#include <stdio.h>
#include <vector>
using namespace std;

#ifdef DEBUG
# define debugf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)
#else
# define debugf(fmt, ...)
#endif
#define errf(fmt, ...) fprintf(stderr, fmt, ##__VA_ARGS__)

struct Node {
    int mn, mn2;
    Node()
        :mn(INT_MAX), mn2(INT_MAX)
    {}
    Node(int mn, int mn2)
        :mn(mn), mn2(mn2)
    {}

    bool valid() const
    {
        return (mn != INT_MAX);
    }
};

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
        data = vector<Node>(base * 2);
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

    Node &at(size_t x)
    {
        return level_accessor(x, 0);
    }
    
    Node &operator[](size_t x)
    {
        return level_accessor(x, 0);
    }

    int query(size_t l, size_t r) const
    {
        return query_internal(l, r, 0, level - 1).mn2;
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

    Node query_internal(size_t l, size_t r, size_t x, size_t lv) const
    {
        size_t lb = lbound(x, lv);
        size_t rb = rbound(x, lv);

        if (l <= lb && rb <= r) {
            return const_accessor(x, lv);
        }

        Node ret = Node();
        size_t m = rbound(x * 2, lv - 1);
        if (l <= m) {
            ret = query_internal(l, r, x * 2, lv - 1);
        }
        if (m < r) {
            Node data = query_internal(l, r, x * 2 + 1, lv - 1);
            if (!ret.valid()) {
                ret = data;
            } else {
                ret = calculate(ret, data);
            }
        }
        return ret;
    }

    inline Node &level_accessor(size_t x, size_t l)
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        debugf("(%zd, %zd) => %d\n", x, l, id);
        return data[id];
    }
    inline const Node &const_accessor(size_t x, size_t l) const
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        debugf("(%zd, %zd) => %d\n", x, l, id);
        return data[id];
    }

    static Node calculate(const Node &l, const Node &r)
    {
        Node ret;
        if (!l.valid() && !r.valid()) {
            return ret;
        }
        ret.mn = min(l.mn, r.mn);
        ret.mn2 = max(l.mn, r.mn);
        ret.mn2 = min(ret.mn2, l.mn2);
        ret.mn2 = min(ret.mn2, r.mn2);
        //printf("(%d, %d), (%d, %d) => (%d, %d)\n", l.mn, l.mn2, r.mn, r.mn2, ret.mn, ret.mn2);
        return ret;
    }

    size_t length, level, base;
    vector<Node> data;
};

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);
    SegmentTree tree(n);
    for (int i = 0; i < n; i++) {
        scanf("%d", &tree[i].mn);
    }
    tree.init_update();

    for (int i = 0; i < m; i++) {
        int a, b;
        scanf("%d%d", &a, &b);
        printf("%d\n", tree.query(a, b));
    }
    return 0;
}


