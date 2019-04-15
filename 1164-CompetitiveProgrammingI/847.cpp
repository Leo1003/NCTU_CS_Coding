#include <iostream>
#include <vector>
#include <queue>
using namespace std;

typedef vector<int>* position;

inline int find_group(const vector<int> &group, int id)
{
    int g = group.at(id - 1);
    return g;
}

inline position find_posi(const vector<position> &list, int group)
{
    position posi = list.at(group - 1);
    return posi;
}

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, q;
    cin >> n >> q;
    vector<bool> event_list(n);
    vector<int> event_group;
    queue<vector<int>> group_queue;
    vector<position> group_position(n, NULL);
    for (int i = 0; i < n; i++) {
        int t;
        cin >> t;
        event_group.push_back(t);
    }

    for (int i = 0; i < q; i++) {
        int type;
        cin >> type;
        if (type == 1) {
            int id;
            cin >> id;
            if (event_list[id - 1]) {
                cout << "Ignore\n";
                continue;
            }

            int group = find_group(event_group, id);
            position pos = find_posi(group_position, group);
            if (pos != NULL) {
                pos->push_back(id);
            } else {
                vector<int> g;
                g.push_back(id);
                group_queue.push(g);
                group_position[group - 1] = &group_queue.back();
            }
            event_list[id - 1] = true;
            cout << "Successful\n";
        } else {
            int id = 0;
            if (group_queue.size() > 0) {
                vector<int> &v = group_queue.front();
                id = v.back();
                v.pop_back();
                
                if (v.size() == 0) {
                    group_position[find_group(event_group, id) - 1] = NULL;
                    group_queue.pop();
                }
        
                event_list[id - 1] = false;
            }
            cout << id << "\n";
        }
    }
    return 0;
}
