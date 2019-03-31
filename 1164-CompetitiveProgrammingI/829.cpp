#include <algorithm>
#include <iostream>
#include <locale>
#include <map>
#include <stdlib.h>
#include <string>
#include <vector>
#include <utility>
using namespace std;

bool comp(pair<string, pair<int, int>> a, pair<string, pair<int, int>> b)
{
    return a.second.first < b.second.first;
}

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    map<string, pair<int, int>> m;
    string s;
    int id = 0;
    while (cin >> s) {
        string word;
        for (size_t i = 0; i < s.size(); i++) {
            if (s[i] >= 'a' && s[i] <= 'z') {
                word += s[i];
            } else if (s[i] >= 'A' && s[i] <= 'Z') {
                word += tolower(s[i]);
            }
        }

        if (word.size() == 0) {
            continue;
        }

        map<string, pair<int, int>>::iterator it = m.find(word);
        if (it == m.end()) {
            pair<int, int> data(id++, 1);
            m[word] = data;
        } else {
            it->second.second++;
        }
    }

    vector<pair<string, pair<int, int>>>vec(m.begin(), m.end());
    sort(vec.begin(), vec.end(), comp);
    for (size_t i = 0; i < vec.size(); i++) {
        cout << vec[i].first << " " << vec[i].second.second << endl;
    }
    return 0;
}
