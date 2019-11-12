#include <algorithm>
#include <iostream>
#define MODULE 524287
using namespace std;

struct Node {
    int x, y;
    Node()
        : x(0)
        , y(0)
    {
    }

    Node(int x, int y)
        : x(x)
        , y(y)
    {
    }

    bool operator==(const Node &other) const
    {
        return (this->x == other.x) && (this->y == other.y);
    }

    friend istream &operator>>(istream &is, Node &node)
    {
        is >> node.x >> node.y;
        return is;
    }
    friend ostream &operator<<(ostream &os, const Node &node)
    {
        os << node.x << " " << node.y;
        return os;
    }
};

int find(const Node &current, const Node &target)
{
    if (current == target) {
        return 1;
    }

    if (current.x < target.x || current.y < target.y) {
        return -1;
    }

    if (current.x > current.y) {
        int res = find(Node(current.x - current.y, current.y), target);
        if (res == -1) {
            return -1;
        } else {
            return (res * 2 + 1) % MODULE;
        }
    } else {
        int res = find(Node(current.x, current.y - current.x), target);
        if (res == -1) {
            return -1;
        } else {
            return (res * 2) % MODULE;
        }
    }
}

int main()
{
    ios_base::sync_with_stdio(false);

    int M;
    cin >> M;
    while (M--) {
        Node root, node;
        cin >> root >> node;
        cout << find(node, root) << endl;
    }
    return 0;
}
