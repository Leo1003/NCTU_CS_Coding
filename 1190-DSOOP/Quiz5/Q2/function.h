#ifndef FUNCTION_H
#define FUNCTION_H
#include <iostream>
#include <sstream>
#include <vector>
#include <utility>
using namespace std;

/* Write Your Code Here */

template<class T>
void add_pair(vector<pair<T, T>> &vec, pair<T, T> pair)
{
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i].first == pair.first && vec[i].second == pair.second) {
            return;
        }
        if (vec[i].first == pair.second && vec[i].second == pair.first) {
            return;
        }
    }
    vec.push_back(pair);
}

template<class T>
string findsum(T arr[15], T sum)
{
    stringstream ss;
    vector<pair<T, T>> v;

    for (int i = 0; i < 15; i++) {
        for (int j = 0; j < 15; j++) {
            if (i != j && arr[i] + arr[j] == sum) {
                add_pair(v, pair<T, T>(arr[i], arr[j]));
            }
        }
    }
    for (size_t i = 0; i < v.size(); i++) {
        ss << v[i].first << "_" << v[i].second << "\n";
    }
    ss << v.size();
    return ss.str();
}



/*----------------------*/

#endif








