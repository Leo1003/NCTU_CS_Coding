#include <algorithm>
#include <ctype.h>
#include <queue>
#include <set>
#include <stdio.h>
using namespace std;

#ifdef _WIN32
#define getcx getchar
#else
#define getcx getchar_unlocked
#endif

const int dir[8][2] = {{1, 2}, {1, -2}, {-1, 2}, {-1, -2}, {2, 1}, {2, -1}, {-2, 1}, {-2, -1}};

char getalpha()
{
    int ch;
    while ((ch = getcx()) != EOF) {
        if (isalpha(ch)) {
            break;
        }
    }
    return ch;
}

struct state {
    state(const char ctable[5][5])
        : panel(0), Wx(-1), Wy(-1)
    {
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                long long value;
                switch (ctable[i][j]) {
                    case 'W':
                        value = 0;
                        Wx = i;
                        Wy = j;
                        break;
                    case 'Y':
                        value = 1;
                        break;
                    case 'P':
                        value = 2;
                        break;
                    case 'B':
                        value = 3;
                        break;
                    default:
                        abort();
                }
                this->panel |= (value << (2 * (i * 5 + j)));
            }
        }
    }

    bool operator == (const state &s) const 
    {
        return this->panel == s.panel;
    }

    inline int get(int x, int y) const
    {
        int loc = 2 * (x * 5 + y);
        return (panel & (0x3LL << loc)) >> loc;
    }
    
    inline void set(int x, int y, int d)
    {
        int loc = 2 * (x * 5 + y);
        panel = (panel & ~(0x3LL << loc)) | ((d & 0x3LL) << loc);
    }

    inline void swap(int x1, int y1, int x2, int y2)
    {
        int d1 = get(x1, y1), d2 = get(x2, y2);
        set(x2, y2, d1);
        set(x1, y1, d2);
        if (d1 == 0) {
            Wx = x2;
            Wy = y2;
        } else if (d2 == 0) {
            Wx = x1;
            Wy = y1;
        }
    }

    long long panel;
    int Wx, Wy;
};

inline bool is_inside(int x, int y, int offsetx, int offsety) {
    int x2 = x + offsetx, y2 = y + offsety;
    return (x2 >= 0 && x2 < 5 && y2 >= 0 && y2 < 5);
}

void BFS_nextlevel(queue<state> &next, const state &current)
{
    if (current.Wx < 0 || current.Wx >= 5 || current.Wy < 0 || current.Wy >= 5) {
        return;
    }
    for (int d = 0; d < 8; d++) {
        if (is_inside(current.Wx, current.Wy, dir[d][0], dir[d][1])) {
            next.emplace(current);
            next.back().swap(current.Wx, current.Wy, current.Wx + dir[d][0], current.Wy + dir[d][1]);
        }
    }
}

int main()
{
    int T;
    scanf("%d", &T);
    while (T--) {
        char table[2][5][5];
        for (int t = 0; t < 2; t++) {
            for (int i = 0; i < 5; i++) {
                for (int j = 0; j < 5; j++) {
                    table[t][i][j] = getalpha();
                }
            }
        }
        
        queue<state> bfs[2];
        bfs[0].emplace(table[0]);
        bfs[1].emplace(table[1]);
        set<long long> walked[2][5][5];

        bool end = false;
        int step = 0;
        while (!end) {
            if (step > 21) {
                // Preventing TLE!!
                abort();
            }
            int count = bfs[step % 2].size();
            while (count--) {
                state &s = bfs[step % 2].front();
                if (!walked[step % 2][s.Wx][s.Wy].count(s.panel)) {
                    if (walked[(step + 1) % 2][s.Wx][s.Wy].count(s.panel)) {
                        printf("%d\n", step - 1);
                        end = true;
                        break;
                    }
                    walked[step % 2][s.Wx][s.Wy].insert(s.panel);
                    BFS_nextlevel(bfs[step % 2], s);
                }
                bfs[step % 2].pop();
            }
            step++;
        }
    }
    return 0;
}

