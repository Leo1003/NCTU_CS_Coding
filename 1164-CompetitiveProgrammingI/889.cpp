#include <iostream>
#include <vector>
using namespace std;

int gcd(int a, int b)
{
    if (a == 0) {
        return b;
    }
    if (b == 0) {
        return a;
    }
    int t;
    while (b > 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

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
        /*
        cerr << "Level: " << level << endl;
        cerr << "Length: " << length << endl;
        cerr << "Base: " << base << endl;
        */
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
                //cerr << "Init: (" << x << ", " << l << ")" << endl;
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
        cerr << "====== SegmentTree Dump ======" << endl;
        for (int l = level - 1; l >= 0; l--) {
            for (int j = 0; j < (1 << (level - l - 1)); j++) {
                cerr << const_accessor(j, l) << " ";
            }
            cerr << endl;
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
        level_accessor(x, l) = gcd(level_accessor(x * 2, l - 1), level_accessor(x * 2 + 1, l - 1));
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
                ret = gcd(ret, data);
            }
        }
        return ret;
    }

    inline int &level_accessor(size_t x, size_t l)
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        //cerr << "(" << x << ", " << l << ") => " << id << endl;
        return data[id];
    }
    inline const int &const_accessor(size_t x, size_t l) const
    {
        size_t l_begin = (1 << (level - l - 1));
        size_t id = l_begin + x;
        //cerr << "(" << x << ", " << l << ") => " << id << endl;
        return data[id];
    }
    size_t length, level, base;
    vector<int> data;
};

int search_right(const SegmentTree &tree, size_t i, int v, size_t l, size_t r)
{
    if (l == r) {
        if (r == tree.size() - 1) {
            if (tree.query(i, r) > v) {
                return -1;
            }
        }
        return l;
    }
    int m = (l + r) / 2;
    if (tree.query(i, m) > v) {
        return search_right(tree, i, v, m + 1, r);
    } else {
        return search_right(tree, i, v, l, m);
    }
}

int main()
{
    ios_base::sync_with_stdio(false);

    int T;
    cin >> T;
    while (T--) {
        int n, q;
        cin >> n >> q;
        SegmentTree tree(n);
        for (int i = 0; i < n; i++) {
            tree[i] = 1;
        }
        tree.init_update();
        //tree.dump();

        for (int k = 0; k < q; k++) {
            int t, i, v;
            cin >> t >> i >> v;
            switch (t) {
                case 1:
                    tree.update(i - 1, v);
                    break;
                case 2:
                    int r = search_right(tree, i - 1, v, i - 1, n - 1);
                    if (r == -1) {
                        cout << "DOYA!" << endl;
                    } else {
                        cout << r + 1 << endl;
                    }
                    break;
            }
            //tree.dump();
        }
    }
}
