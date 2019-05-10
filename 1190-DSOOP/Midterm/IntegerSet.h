#include <iostream>
#include <string>
#include <vector>
#include <limits.h>
#ifndef INTEGERSET_H
#define INTEGERSET_H
using namespace std;

/*----Write Your Answer Here-----*/

class IntegerSet {
public:
    IntegerSet(int size)
        :data(size, false)
    {}

    void InsertElement(int value)
    {
        if (value >= data.size()) {
            return;
        }
        data[value] = true;
    }

    void printSet()
    {
        for (int i = 0; i < data.size(); i++) {
            if (data[i]) {
                cout << i << "_";
            }
        }
    }

    friend int highest(const IntegerSet *set, int size);
    friend int lowest(const IntegerSet *set, int size);

protected:
    vector<bool> data;
};

class EvenIntegerSet: public IntegerSet {
public:
    EvenIntegerSet(int size)
        :IntegerSet(size)
    {}

    void filterSet() {
        for (int i = 0; i < data.size(); i += 2) {
            if (data[i]) {
                cout << i << "_";
            }
        }
    }
};

int highest(const IntegerSet *set, int size) {
    int highest = 0;
    for (int i = 0; i < size; i++) {
        if (set->data[i]) {
            highest = i;
        }
    }
    return highest;
}

int lowest(const IntegerSet *set, int size) {
    int lowest = INT_MAX;
    for (int i = size - 1; i >= 0; i--) {
        if (set->data[i]) {
            lowest = i;
        }
    }
    return lowest;
}

#endif


