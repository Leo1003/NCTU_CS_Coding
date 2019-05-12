#include <iostream>
#include <vector>
using namespace std;

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
        return data[x];
    }
    
    void update(size_t x, int value)
    {
        if (data[x] == value) {
            return;
        }
        data[x] = value;

        for (size_t l = 1; l < level; l++) {
            size_t div = 1 << l;
            size_t l_x = x / div;
            renew(l_x, l);
        }
    }

    int search_nth_small(int nth)
    {
        int cur_l = level - 1;
        if (level_accessor(0, cur_l) < nth) {
            return -1;
        }

        size_t cur_x = 0;
        int pre = 0;
        for (; cur_l >= 1; cur_l--) {
            if (level_accessor(cur_x * 2, cur_l - 1) + pre >= nth) {
                cur_x = cur_x * 2;
            } else {
                pre += level_accessor(cur_x * 2, cur_l - 1);
                cur_x = cur_x * 2 + 1;
            }
        }
        return cur_x;
    }

    int &operator[](size_t x)
    {
        return data[x];
    }

    void dump() {
        cerr << "====== SegmentTree Dump ======" << endl;
        for (int l = level - 1; l >= 0; l--) {
            for (int j = 0; j < (1 << (level - l - 1)); j++) {
                cerr << level_accessor(j, l) << " ";
            }
            cerr << endl;
        }
    }

private:
    void renew(size_t x, size_t l)
    {
        level_accessor(x, l) = level_accessor(x * 2, l - 1) + level_accessor(x * 2 + 1, l - 1);
    }

    int &level_accessor(size_t x, size_t l)
    {
        size_t l_begin = data.size() - (1 << (level - l));
        size_t id = l_begin + x;
        //cerr << "(" << x << ", " << l << ") => " << id << endl;
        return data[id];
    }
    size_t length, level, base;
    vector<int> data;
};

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

        for (int i = 0; i < q; i++) {
            int t, x;
            cin >> t >> x;
            switch (t) {
                case 1:
                    if (tree[x - 1] > 0) {
                        tree.update(x - 1, tree[x - 1] + 1);
                    }
                    break;
                case 2:
                    if (tree[x - 1] > 0) {
                        tree.update(x - 1, 1);
                    }
                    break;
                case 3:
                    int id = tree.search_nth_small(x);
                    if (id < 0) {
                        cout << "endro~!" << endl;
                    } else {
                        cout << id + 1 << endl;
                        tree.update(id, tree[id] - 1);
                    }
                    break;
            }
            //tree.dump();
        }
    }
}
