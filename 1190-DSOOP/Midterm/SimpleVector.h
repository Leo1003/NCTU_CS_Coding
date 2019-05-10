#include <iostream>
#include <cmath>
#include <cstring>
using namespace std;

/*----Write Your Answer Here-----*/
template<class T>
class SimpleVector {
public:
    SimpleVector(int size = 5)
        : capacity(size), size(0)
    {
        this->data = new T[this->capacity];
    }

    ~SimpleVector()
    {
        delete[] data;
    }

    int my_empty()
    {
        return (size == 0 ? 1 : 0);
    }

    int my_size()
    {
        return size;
    }

    void my_reverse()
    {
        for (int i = 0; i < size / 2; i++) {
            T tmp = data[i];
            data[i] = data[size - i - 1];
            data[size - i - 1] = tmp;
        }
    }

    void my_clear()
    {
        size = 0;
    }

    void my_extend()
    {
        int new_capacity = capacity * 2;
        T *new_data = new T[new_capacity];
        if (!new_data) {
            return;
        }

        memcpy(new_data, data, sizeof(T) * size);
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }

    void my_push_back(T value)
    {
        while (size >= capacity) {
            my_extend();
        }
        data[size] = value;
        size++;
    }

    void my_pop_back()
    {
        if (size == 0) {
            return;
        }
        size--;
    }

    void my_insert(T value, int index)
    {
        if (index < 0 || index > size) {
            return;
        }
        if (index == size) {
            my_push_back(value);
        } else {
            while (size >= capacity) {
                my_extend();
            }
            memmove(data + index + 1, data + index, sizeof(T) * (size - index));
            size++;
            data[index] = value;
        }
    }

    void my_set(T value, int index)
    {
        if (index < 0 || index >= size) {
            return;
        }
        data[index] = value;
    }

    void print()
    {
        for (int i = 0; i < size; i++) {
            cout << data[i] << "_";
        }
    }

    T operator[](int index)
    {
        if (index < 0 || index >= size) {
            if (size == 0) {
                return 0;
            }
            return data[size - 1];
        }
        return data[index];
    }
private:
    int capacity, size;
    T *data;
};





