#include <iostream>
#include <string>
using namespace std;

class IntegerSet{
    public:
        IntegerSet(int);
        ~IntegerSet(){};
        IntegerSet Union_of_Set(IntegerSet);
        IntegerSet Intersection_of_Set(IntegerSet);
        void InsertElement(int);
        void DeleteElement(int);
        bool isEqualTo(IntegerSet);
        void printSet();
    private:
        bool set[100];
        int size;
};

IntegerSet::IntegerSet(int size)
{
    this->size = size;
    for (int i = 0; i < 100; i++) {
        set[i] = false;
    }
}

void IntegerSet::InsertElement(int i)
{
    if (i < 0 || i >= size) {
        return;
    }
    set[i] = true;
}

void IntegerSet::DeleteElement(int i)
{
    if (i < 0 || i >= size) {
        return;
    }
    set[i] = false;
}

bool IntegerSet::isEqualTo(IntegerSet other)
{
    if (this->size != other.size) {
        return false;
    }
    for (int i = 0; i < this->size; i++) {
        if (this->set[i] != other.set[i]) {
            return false;
        }
    }
    return true;
}

IntegerSet IntegerSet::Union_of_Set(IntegerSet other)
{
    IntegerSet ret(100);
    for (int i = 0; i < 100; i++) {
        ret.set[i] = this->set[i] || other.set[i];
    }
    return ret;
}

IntegerSet IntegerSet::Intersection_of_Set(IntegerSet other)
{
    IntegerSet ret(100);
    for (int i = 0; i < 100; i++) {
        ret.set[i] = this->set[i] && other.set[i];
    }
    return ret;
}

void IntegerSet::printSet()
{
    for (int i = 0; i < 100; i++) {
        if (set[i]) {
            cout << i << "_";
        }
    }
}