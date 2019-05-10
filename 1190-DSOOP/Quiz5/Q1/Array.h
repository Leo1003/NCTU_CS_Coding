#ifndef ARRAY_H
#define ARRAY_H
#include <iostream>
using namespace std;

template<class T>
class Array
{
public:
    Array(T *ptr, size_t len)
        : pointer(ptr), length(len) {}
    ~Array() {}
    T find_max();
    T find_min();
    int seek_index(T value);
    T calculate();
    void display();
private:
    T *pointer;
    size_t length;
};

template<class T>
T Array<T>::find_max()
{
    T mx = pointer[0];
    for (size_t i = 1; i < length; i++) {
        if (pointer[i] > mx) {
            mx = pointer[i];
        }
    }
    return mx;
}

template<class T>
T Array<T>::find_min()
{
    T mn = pointer[0];
    for (size_t i = 1; i < length; i++) {
        if (pointer[i] < mn) {
            mn = pointer[i];
        }
    }
    return mn;
}

template<class T>
int Array<T>::seek_index(T value)
{
    for (size_t i = 0; i < length; i++) {
        if (pointer[i] == value) {
            return i;
        }
    }
    return -1;
}

template<class T>
T Array<T>::calculate()
{
    T odd = 0, even = 0;
    for (size_t i = 0; i < length; i++) {
        if (i % 2) {
            even += pointer[i];
        } else {
            odd += pointer[i];
        }
    }
    return odd - even;
}

template<class T>
void Array<T>::display()
{
    for (size_t i = 0; i < length; i++) {
        if (i > 0) {
            cout << "_";
        }
        cout << pointer[i];
    }
    cout << endl;
}

#endif
